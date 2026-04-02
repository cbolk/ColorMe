/* WebSocket Configuration
   window.location.hostname automatically detects the ESP32 IP address 
*/
let gateway = `ws://${window.location.hostname}/ws`;
let websocket;

// Initialize when the page loads
document.addEventListener('DOMContentLoaded', () => {
    initWebSocket();
    initWifiConfig();
});

// --- COMMUNICATION MANAGEMENT (WEBSOCKET) ---

function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);

    websocket.onopen = () => {
        console.log('Connection opened');
        showStatus("System Online", "success");
    };

    websocket.onclose = () => {
        console.log('Connection closed. Reconnecting in 2 seconds...');
        showStatus("Reconnecting...", "loading");
        setTimeout(initWebSocket, 2000);
    };

    websocket.onmessage = (event) => {
        try {
            const data = event.data;
            
            // 1. Handle Color Update (Incoming JSON)
            if (data.startsWith("COLOR_JSON:")) {
                const jsonStr = data.substring(11);
                const color = JSON.parse(jsonStr);
                updateColorUI(color.r, color.g, color.b, color.name);
            }
            
            // 2. Handle WiFi Status Update
            if (data.startsWith("STATUS:")) {
                const status = data.substring(7);
                handleWifiStatus(status);
            }
        } catch (e) {
            console.error("Error parsing WebSocket data:", e);
        }
    };
}

// --- USER INTERFACE LOGIC (UI) ---

function updateColorUI(r, g, b, name) {
    document.getElementById('val-r').innerText = r;
    document.getElementById('val-g').innerText = g;
    document.getElementById('val-b').innerText = b;
    document.getElementById('color-name').innerText = name;
    
    const box = document.getElementById('color-box');
    box.style.backgroundColor = `rgb(${r}, ${g}, ${b})`;
}

function openTab(tabName) {
    const tabsContainer = document.querySelector('.tabs');
    tabsContainer.setAttribute('data-active', tabName);

    const buttons = document.querySelectorAll('.tab-btn');
    buttons.forEach(btn => btn.classList.remove('active'));
    
    if(tabName === 'config') {
        buttons[0].classList.add('active');
        document.getElementById('tab-config').style.display = 'flex';
        document.getElementById('tab-color').style.display = 'none';
    } else {
        buttons[1].classList.add('active');
        document.getElementById('tab-config').style.display = 'none';
        document.getElementById('tab-color').style.display = 'flex';
    }
}

// --- WIFI CONFIGURATION LOGIC ---

function initWifiConfig() {
    const saveBtn = document.getElementById('saveBtn');
    
    saveBtn.addEventListener('click', () => {
        const ssid = document.getElementById('ssid').value.trim();
        const psw = document.getElementById('psw').value.trim();

        if (!ssid) {
            alert("SSID name is required");
            return;
        }

        saveBtn.disabled = true;
        showStatus("Sending credentials...", "loading", true);

        // Create JSON object to send via WebSocket
        const wifiConfig = {
            type: "SET_WIFI",
            ssid: ssid,
            psw: psw
        };

        websocket.send(JSON.stringify(wifiConfig));
    });
}

function handleWifiStatus(status) {
    const saveBtn = document.getElementById('saveBtn');
    saveBtn.disabled = false;
    
    if (status === 'connected') {
        showStatus("Success! Device is Connecting...", "success");
    } else {
        showStatus("Connection Error", "error");
    }
}

// --- UTILITIES ---

function showStatus(msg, type, loader = false) {
    const box = document.getElementById('status-box');
    const text = document.getElementById('status-text');
    const loaderEl = document.getElementById('loader');

    box.className = `status-${type}`; 
    box.classList.remove('hidden');
    text.innerHTML = msg;
    
    if(loaderEl) {
        loaderEl.style.display = loader ? 'block' : 'none';
    }
}

function togglePassword() {
    const psw = document.getElementById('psw');
    psw.type = psw.type === "password" ? "text" : "password";
}

window.openTab = openTab;