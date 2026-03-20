/* WebSocket Configuration */
let gateway = `ws://${window.location.hostname}/ws`;
let websocket;

// Inizializzazione al caricamento della pagina
document.addEventListener('DOMContentLoaded', () => {
    initWebSocket();
    initWifiConfig();
});

// --- GESTIONE COMUNICAZIONE (WEBSOCKET) ---

function initWebSocket() {
    console.log('Tentativo di apertura connessione WebSocket...');
    websocket = new WebSocket(gateway);

    websocket.onopen = () => {
        console.log('Connessione stabilita con successo');
        showStatus("System Online", "success");
    };

    websocket.onclose = () => {
        console.log('Connessione chiusa. Riconnessione in 2 secondi...');
        showStatus("Reconnecting...", "loading");
        setTimeout(initWebSocket, 2000);
    };

    websocket.onmessage = (event) => {
        const data = event.data;
        // console.log("Dati ricevuti dall'ESP32:", data); // Scommenta per debug estremo

        // 1. Gestione Aggiornamento Colore (Messaggio: COLOR_JSON:{"r":...})
        if (data.startsWith("COLOR_JSON:")) {
            try {
                // Estraiamo il JSON rimuovendo il prefisso "COLOR_JSON:"
                const jsonStr = data.substring(11).trim();
                const color = JSON.parse(jsonStr);
                
                console.log("Nuovo colore ricevuto:", color);
                
                // Aggiorniamo l'interfaccia
                updateColorUI(color.r, color.g, color.b, color.name);
            } catch (e) {
                console.error("Errore nel parsing del JSON del colore:", e);
            }
        }
        
        // 2. Gestione Stato WiFi (Messaggio: STATUS:connected)
        else if (data.startsWith("STATUS:")) {
            const status = data.substring(7).trim();
            handleWifiStatus(status);
        }
    };

    websocket.onerror = (error) => {
        console.error("Errore WebSocket:", error);
    };
}

// --- LOGICA INTERFACCIA UTENTE (UI) ---

function updateColorUI(r, g, b, name) {
    // Aggiornamento testi
    document.getElementById('val-r').innerText = r;
    document.getElementById('val-g').innerText = g;
    document.getElementById('val-b').innerText = b;
    document.getElementById('color-name').innerText = name || "Sconosciuto";
    
    // Aggiornamento anteprima visiva
    const box = document.getElementById('color-box');
    if (box) {
        box.style.backgroundColor = `rgb(${r}, ${g}, ${b})`;
    }
}

// --- GESTIONE CONFIGURAZIONE WIFI ---

function initWifiConfig() {
    const saveBtn = document.getElementById('saveBtn');
    if (!saveBtn) return;

    saveBtn.addEventListener('click', () => {
        const ssid = document.getElementById('ssid').value.trim();
        const psw = document.getElementById('psw').value.trim();

        if (!ssid) {
            alert("Il nome SSID è obbligatorio");
            return;
        }

        saveBtn.disabled = true;
        showStatus("Invio credenziali...", "loading", true);

        // Invio comando SET_WIFI via WebSocket
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
    if (saveBtn) saveBtn.disabled = false;
    
    if (status === 'connected') {
        showStatus("Successo! Connessione in corso...", "success");
    } else {
        showStatus("Errore di connessione", "error");
    }
}

// --- UTILITY E TABS ---

function openTab(tabName) {
    const buttons = document.querySelectorAll('.tab-btn');
    const configTab = document.getElementById('tab-config');
    const colorTab = document.getElementById('tab-color');

    buttons.forEach(btn => btn.classList.remove('active'));

    if (tabName === 'config') {
        buttons[0].classList.add('active');
        if (configTab) configTab.style.display = 'flex';
        if (colorTab) colorTab.style.display = 'none';
    } else {
        buttons[1].classList.add('active');
        if (configTab) configTab.style.display = 'none';
        if (colorTab) colorTab.style.display = 'flex';
    }
}

function showStatus(msg, type, loader = false) {
    const box = document.getElementById('status-box');
    const text = document.getElementById('status-text');
    const loaderEl = document.getElementById('loader');

    if (box && text) {
        box.className = `status-${type}`; 
        box.classList.remove('hidden');
        text.innerHTML = msg;
        if (loaderEl) loaderEl.style.display = loader ? 'block' : 'none';
    }
}

function togglePassword() {
    const psw = document.getElementById('psw');
    if (psw) psw.type = psw.type === "password" ? "text" : "password";
}

// Rendiamo openTab globale per gli attributi onclick dell'HTML
window.openTab = openTab;