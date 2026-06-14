// =============================================================================
// RuntimeConfigPanel -- optional config JSON loader for the web UI.
// =============================================================================

import { UIComponent } from '../UIComponent.js';

const DEFAULT_CONFIG_URL = 'config/factory_config.runtime.json';

export class RuntimeConfigPanel extends UIComponent {
    #ctrl;
    #onLoaded;
    #status;

    constructor(controller, onLoaded) {
        super();
        this.#ctrl = controller;
        this.#onLoaded = onLoaded;
    }

    bind() {
        this.#status = document.getElementById('config-status');

        document.getElementById('btn-load-runtime-config').addEventListener('click', async () => {
            await this.#loadDefaultConfig();
        });

        document.getElementById('config-file-input').addEventListener('change', async e => {
            const file = e.target.files && e.target.files[0];
            if (!file) return;
            await this.#loadFile(file);
            e.target.value = '';
        });
    }

    render(_snap) {}

    async #loadDefaultConfig() {
        try {
            this.#setStatus('Loading runtime config...', 'pending');
            const response = await fetch(`${DEFAULT_CONFIG_URL}?v=${Date.now()}`, { cache: 'no-store' });
            if (!response.ok) {
                throw new Error(`HTTP ${response.status}`);
            }
            await this.#loadJsonText(await response.text(), 'Runtime config loaded');
        } catch (error) {
            this.#fail(error);
        }
    }

    async #loadFile(file) {
        try {
            this.#setStatus(`Loading ${file.name}...`, 'pending');
            await this.#loadJsonText(await file.text(), `${file.name} loaded`);
        } catch (error) {
            this.#fail(error);
        }
    }

    async #loadJsonText(jsonText, successMessage) {
        const ok = this.#ctrl.loadFactoryConfigFromString(jsonText);
        if (!ok) {
            throw new Error('Controller rejected config');
        }
        this.#setStatus(successMessage, 'success');
        this.#onLoaded();
    }

    #fail(error) {
        console.error('[gactorio] config load failed', error);
        this.#setStatus(`Failed to load config: ${error.message || error}`, 'error');
    }

    #setStatus(message, state) {
        if (!this.#status) return;
        this.#status.textContent = message;
        this.#status.dataset.state = state;
    }
}
