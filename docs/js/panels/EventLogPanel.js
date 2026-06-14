// =============================================================================
// EventLogPanel — Observer-pattern event stream, most recent first.
// (Bottom of the left column; scrolls within its own box.)
// =============================================================================

import { UIComponent } from '../UIComponent.js';
import { esc } from '../util.js';

export class EventLogPanel extends UIComponent {
    #ctrl;

    constructor(controller) {
        super();
        this.#ctrl = controller;
    }

    bind() {
        document.getElementById('btn-clear-log').addEventListener('click', () => {
            this.#ctrl.clearEventLog();
        });
    }

    render(snap) {
        const events = snap.events;
        let html = '';
        for (let i = events.length - 1; i >= 0; i--) {
            const e = events[i];
            html += `<div class="event">
                <span class="time">${e.time.toFixed(2)}s</span>
                <span class="type">${esc(e.typeName)}</span>
                <span class="msg">${esc(e.message)}</span>
            </div>`;
        }
        document.getElementById('event-log').innerHTML =
            html || '<div style="color:#666">(no events yet)</div>';
    }
}
