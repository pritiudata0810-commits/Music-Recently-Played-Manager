/**
 * STACKSOUND: Frontend Application Logic
 * Communicates strictly with C++ HTTP Backend (127.0.0.1:8080)
 * Demonstrates LIFO Stack Operations
 */

// Detect API base URL: defaults to window.location.origin if loaded from server, or 127.0.0.1:8000
let API_BASE = (window.location.protocol.startsWith('http')) 
    ? window.location.origin 
    : 'http://127.0.0.1:8000';

// Global application state mirrored strictly from C++
let songLibrary = [];
let currentState = {
    hasCurrentSong: false,
    currentSong: null,
    backHistory: [],
    forwardHistory: [],
    backHistorySize: 0,
    forwardHistorySize: 0,
    capacity: 25,
    lastResult: null
};

let isRequestPending = false;
let isSimulatedPlaying = false;

// DOM Elements
const backendStatusPill = document.getElementById('backendStatusPill');
const backendStatusText = document.getElementById('backendStatusText');
const floatingCarousel = document.getElementById('floatingCarousel');
const songGrid = document.getElementById('songGrid');
const recentTableBody = document.getElementById('recentTableBody');
const recentCountHint = document.getElementById('recentCountHint');
const stackVisualizerContainer = document.getElementById('stackVisualizerContainer');
const stackEmptyState = document.getElementById('stackEmptyState');
const stackSizeDisplay = document.getElementById('stackSizeDisplay');
const forwardSizeDisplay = document.getElementById('forwardSizeDisplay');
const latestOpDisplay = document.getElementById('latestOpDisplay');
const terminalLog = document.getElementById('terminalLog');

// Player DOM
const playerArt = document.getElementById('playerArt');
const playerTitle = document.getElementById('playerTitle');
const playerArtist = document.getElementById('playerArtist');
const playbackBadge = document.getElementById('playbackBadge');
const playerTimer = document.getElementById('playerTimer');
const prevBtn = document.getElementById('prevBtn');
const nextBtn = document.getElementById('nextBtn');
const playToggleBtn = document.getElementById('playToggleBtn');
const playIcon = document.getElementById('playIcon');
const bottomPlayer = document.querySelector('.bottom-player');
const peekBtn = document.getElementById('peekBtn');
const clearBtn = document.getElementById('clearBtn');
const bannerArt = document.getElementById('bannerArt');

// Initialize application on page load
window.addEventListener('DOMContentLoaded', async () => {
    setupEventListeners();
    await checkBackendAndLoad();
});

// Setup click and navigation event listeners
function setupEventListeners() {
    prevBtn.addEventListener('click', () => sendAction('/api/previous'));
    nextBtn.addEventListener('click', () => sendAction('/api/next'));
    peekBtn.addEventListener('click', () => sendAction('/api/peek'));
    
    clearBtn.addEventListener('click', () => {
        if (confirm('Are you sure you want to clear Back and Forward history stacks?\n(The currently active song will remain selected).')) {
            sendAction('/api/clear');
        }
    });

    playToggleBtn.addEventListener('click', toggleSimulatedPlayback);

    // Sidebar navigation tabs
    const navItems = document.querySelectorAll('.nav-item');
    navItems.forEach(item => {
        item.addEventListener('click', () => {
            navItems.forEach(n => n.classList.remove('active'));
            item.classList.add('active');
            const tab = item.dataset.tab;
            if (tab === 'songs') {
                document.getElementById('songsSection').scrollIntoView({ behavior: 'smooth' });
            } else if (tab === 'recently-played') {
                document.getElementById('recentSection').scrollIntoView({ behavior: 'smooth' });
            } else if (tab === 'visualizer') {
                document.querySelector('.stack-panel').scrollIntoView({ behavior: 'smooth' });
            } else {
                window.scrollTo({ top: 0, behavior: 'smooth' });
            }
        });
    });
}

// Initial health check and load
async function checkBackendAndLoad() {
    try {
        updateBackendStatus('checking', 'Connecting to C++ Backend...');
        const songsResp = await fetch(`${API_BASE}/api/songs`);
        if (!songsResp.ok) throw new Error('Failed to fetch songs');
        songLibrary = await songsResp.json();

        const stateResp = await fetch(`${API_BASE}/api/state`);
        if (!stateResp.ok) throw new Error('Failed to fetch state');
        currentState = await stateResp.json();

        updateBackendStatus('online', 'C++ Backend Connected (Port 8080)');
        renderLibrary();
        renderState(currentState);
    } catch (err) {
        console.error('Connection error:', err);
        updateBackendStatus('error', 'C++ Backend Offline (Run start_backend.bat)');
        terminalLog.textContent = `[!] Error connecting to C++ backend at ${API_BASE}\n` +
                                  `Please start the C++ server by running:\n` +
                                  `   start_backend.bat\n` +
                                  `or compile and execute backend_server.exe`;
    }
}

// Update header status indicator
function updateBackendStatus(state, text) {
    const dot = backendStatusPill.querySelector('.status-dot');
    dot.className = 'status-dot';
    if (state === 'online') dot.classList.add('online');
    else if (state === 'error') dot.classList.add('error');
    backendStatusText.textContent = text;
}

// Send user action to C++ backend with debouncing
async function sendAction(endpoint) {
    if (isRequestPending) return;
    isRequestPending = true;

    try {
        const resp = await fetch(`${API_BASE}${endpoint}`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' }
        });
        if (!resp.ok) throw new Error(`HTTP error ${resp.status}`);
        const newState = await resp.json();
        currentState = newState;
        renderState(currentState);
    } catch (err) {
        console.error(`Action to ${endpoint} failed:`, err);
        updateBackendStatus('error', 'Connection lost to C++ backend');
    } finally {
        isRequestPending = false;
    }
}

// Play specific song by ID
function playSong(songId) {
    sendAction(`/api/play?id=${encodeURIComponent(songId)}`);
}

// Toggle simulated playback
function toggleSimulatedPlayback() {
    if (!currentState.hasCurrentSong) {
        if (songLibrary.length > 0) {
            playSong(songLibrary[0].id);
        }
        return;
    }
    isSimulatedPlaying = !isSimulatedPlaying;
    if (isSimulatedPlaying) {
        bottomPlayer.classList.add('playing');
        playIcon.innerHTML = '<rect x="6" y="4" width="4" height="16"></rect><rect x="14" y="4" width="4" height="16"></rect>';
        playbackBadge.textContent = 'Simulated Playing';
        playbackBadge.style.color = '#2ecc71';
    } else {
        bottomPlayer.classList.remove('playing');
        playIcon.innerHTML = '<polygon points="5 3 19 12 5 21 5 3"></polygon>';
        playbackBadge.textContent = 'Simulated Paused';
        playbackBadge.style.color = '#f1c40f';
    }
}

// Render song library (both 3D Carousel and Song Grid)
function renderLibrary() {
    floatingCarousel.innerHTML = '';
    songGrid.innerHTML = '';

    songLibrary.forEach((song, idx) => {
        // 1. Floating Carousel Card
        const floatCard = document.createElement('div');
        floatCard.className = 'float-card';
        floatCard.id = `card-${song.id}`;
        floatCard.innerHTML = `
            <div class="float-art-wrap">
                <img src="${song.artworkUrl}" alt="${song.title}" loading="lazy">
            </div>
            <div class="float-info">
                <span class="float-title">${song.title}</span>
                <span class="float-artist">${song.artist}</span>
            </div>
        `;
        floatCard.addEventListener('click', () => playSong(song.id));
        floatingCarousel.appendChild(floatCard);

        // 2. Song Grid Item
        const gridCard = document.createElement('div');
        gridCard.className = 'song-row-card';
        gridCard.id = `grid-${song.id}`;
        gridCard.innerHTML = `
            <div class="song-thumb">
                <img src="${song.artworkUrl}" alt="${song.title}" loading="lazy">
            </div>
            <div class="song-meta">
                <div class="song-row-title">${song.title}</div>
                <div class="song-row-artist">${song.artist}</div>
            </div>
            <div class="song-row-duration">${song.duration}</div>
            <div class="play-action-icon" title="Play ${song.title}">
                <svg width="14" height="14" viewBox="0 0 24 24" fill="currentColor"><polygon points="5 3 19 12 5 21 5 3"></polygon></svg>
            </div>
        `;
        gridCard.addEventListener('click', () => playSong(song.id));
        songGrid.appendChild(gridCard);
    });
}

// Render complete authoritative state from C++ backend
function renderState(state) {
    // 1. Active current song display
    if (state.hasCurrentSong && state.currentSong) {
        const song = state.currentSong;
        playerTitle.textContent = song.title;
        playerArtist.textContent = `${song.artist} • ${song.album}`;
        playerArt.src = song.artworkUrl;
        bannerArt.src = song.artworkUrl;
        playerTimer.textContent = `0:00 / ${song.duration}`;
        playbackBadge.textContent = 'Now Selected';
        playbackBadge.style.color = '#f1c40f';

        // Highlight active items
        document.querySelectorAll('.float-card').forEach(c => c.classList.remove('active-card'));
        document.querySelectorAll('.song-row-card').forEach(c => c.classList.remove('active-song'));
        
        const activeCard = document.getElementById(`card-${song.id}`);
        if (activeCard) activeCard.classList.add('active-card');
        const activeGrid = document.getElementById(`grid-${song.id}`);
        if (activeGrid) activeGrid.classList.add('active-song');
    } else {
        playerTitle.textContent = 'Select a song to play';
        playerArtist.textContent = 'Predefined Bollywood Library';
        playerArt.src = 'assets/default.svg';
        bannerArt.src = 'assets/song_1.svg';
        playbackBadge.textContent = 'Ready';
        playbackBadge.style.color = '#888';
    }

    // 2. Metrics & Badges
    stackSizeDisplay.textContent = `${state.backHistorySize} / ${state.capacity}`;
    forwardSizeDisplay.textContent = state.forwardHistorySize;
    if (state.lastResult && state.lastResult.operation) {
        latestOpDisplay.textContent = state.lastResult.operation;
    }

    // 3. Navigation Controls Availability
    prevBtn.disabled = (state.backHistorySize === 0);
    nextBtn.disabled = (state.forwardHistorySize === 0);

    // 4. Render Recently Played Table
    recentCountHint.textContent = `${state.backHistorySize} songs in Back History (LIFO)`;
    recentTableBody.innerHTML = '';
    if (!state.backHistory || state.backHistory.length === 0) {
        recentTableBody.innerHTML = `
            <tr class="empty-row">
                <td colspan="6">No songs in back history yet. Play some songs from the library above!</td>
            </tr>
        `;
    } else {
        state.backHistory.forEach((song, idx) => {
            const tr = document.createElement('tr');
            const isTop = (idx === 0);
            tr.innerHTML = `
                <td>
                    <span class="pos-badge ${isTop ? 'top-badge' : ''}">${isTop ? 'TOP' : '#' + (idx + 1)}</span>
                </td>
                <td>
                    <div style="display:flex; align-items:center; gap:10px;">
                        <img src="${song.artworkUrl}" style="width:32px; height:32px; border-radius:4px; object-fit:cover;">
                        <strong>${song.title}</strong>
                    </div>
                </td>
                <td>${song.artist}</td>
                <td>${song.album}</td>
                <td style="text-align: right;">${song.duration}</td>
                <td style="text-align: center;">
                    <button class="header-action-btn" style="padding:3px 10px; font-size:11px;" onclick="playSong('${song.id}')">Select</button>
                </td>
            `;
            recentTableBody.appendChild(tr);
        });
    }

    // 5. Render Right Stack Visualizer Panel (LIFO Vertical Display)
    stackVisualizerContainer.innerHTML = '';
    if (!state.backHistory || state.backHistory.length === 0) {
        stackVisualizerContainer.appendChild(stackEmptyState);
    } else {
        state.backHistory.forEach((song, idx) => {
            const isTop = (idx === 0);
            const node = document.createElement('div');
            node.className = `stack-node ${isTop ? 'is-top' : ''}`;
            node.innerHTML = `
                <div class="node-pointer">${isTop ? 'TOP ➔' : `[ ${state.backHistory.length - 1 - idx} ]`}</div>
                <div class="node-thumb">
                    <img src="${song.artworkUrl}" alt="${song.title}">
                </div>
                <div class="node-info">
                    <div class="node-title">${song.title}</div>
                    <div class="node-artist">${song.artist}</div>
                </div>
            `;
            stackVisualizerContainer.appendChild(node);
        });
    }

    // 6. Update Mirrored Terminal Log Box
    if (state.lastResult) {
        const lr = state.lastResult;
        terminalLog.textContent = 
`========================================
       STACKSOUND - C++ BACKEND
========================================
Action:        ${lr.action || 'STATUS'}
Operation:     ${lr.operation || 'SYNC'}
Status:        ${lr.success ? 'SUCCESS' : 'FAILED / UNDERFLOW'}
Details:       ${lr.message || 'State synchronized successfully.'}
Affected Item: ${lr.affectedSongTitle || 'None'}
Current Song:  ${state.hasCurrentSong ? state.currentSong.title : '[None]'}
Back Stack:    ${state.backHistorySize} / ${state.capacity} (LIFO)
Forward Stack: ${state.forwardHistorySize}
========================================`;
    }
}
