// =============================================================================
// InventoryPanel — raw items and finished products. (Right column.)
// =============================================================================

import { UIComponent } from '../UIComponent.js';
import { esc } from '../util.js';

export class InventoryPanel extends UIComponent {
    render(snap) {
        const rows = snap.inventory
            .map(it => `<tr><td>${esc(it.id)}</td><td>${it.quantity}</td></tr>`)
            .join('');
        document.getElementById('inventory-content').innerHTML = `
            <table>
                <thead><tr><th>Item</th><th>Qty</th></tr></thead>
                <tbody>${rows || '<tr><td colspan="2" style="color:#666">(empty)</td></tr>'}</tbody>
            </table>`;
    }
}
