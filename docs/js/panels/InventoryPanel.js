// =============================================================================
// InventoryPanel — raw items and finished products. (Right column.)
// =============================================================================

import { UIComponent } from '../UIComponent.js';
import { esc } from '../util.js';

export class InventoryPanel extends UIComponent {
    #ctrl;

    constructor(controller) {
        super();
        this.#ctrl = controller;
    }

    bind() {
        document.getElementById('inventory-content').addEventListener('pointerdown', e => {
            const btn = e.target.closest('button[data-restock-item]');
            if (!btn || btn.disabled) return;

            const itemId = parseInt(btn.dataset.restockItem, 10);
            console.log('[gactorio] restockItem', itemId, '→', this.#ctrl.restockItem(itemId));
        });
    }

    render(snap) {
        const rawItems = snap.inventory.filter(it => !it.isProduct);
        const products = snap.inventory.filter(it => it.isProduct);

        document.getElementById('inventory-content').innerHTML = `
            <h3>Raw Items</h3>
            <table>
                <thead><tr><th>Item</th><th>Qty</th><th></th></tr></thead>
                <tbody>${this.#rawRows(rawItems)}</tbody>
            </table>
            <h3>Finished Products</h3>
            <table>
                <thead><tr><th>Product</th><th>Qty</th></tr></thead>
                <tbody>${this.#productRows(products)}</tbody>
            </table>`;
    }

    #rawRows(items) {
        if (items.length === 0) {
            return '<tr><td colspan="3" style="color:#666">(empty)</td></tr>';
        }

        return items.map(it => `
            <tr>
                <td>${esc(it.name)}</td>
                <td>${it.quantity}</td>
                <td><button class="small restock" data-restock-item="${it.id}">+5</button></td>
            </tr>`).join('');
    }

    #productRows(products) {
        if (products.length === 0) {
            return '<tr><td colspan="2" style="color:#666">(none yet)</td></tr>';
        }

        return products.map(it => `
            <tr>
                <td>${esc(it.name)}</td>
                <td>${it.quantity}</td>
            </tr>`).join('');
    }
}
