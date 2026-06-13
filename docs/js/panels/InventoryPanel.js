// =============================================================================
// InventoryPanel - raw items and finished products. (Right column.)
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
        const restock = (btn) => {
            const itemId = btn.dataset.restockItem;
            console.log('[gactorio] restockItemById', itemId, '->', this.#ctrl.restockItemById(itemId));
        };

        document.getElementById('inventory-content').addEventListener('pointerdown', e => {
            const btn = e.target.closest('button[data-restock-item]');
            if (!btn || btn.disabled) return;
            e.preventDefault();
            btn.dataset.pointerHandled = '1';
            restock(btn);
        });

        document.getElementById('inventory-content').addEventListener('click', e => {
            const btn = e.target.closest('button[data-restock-item]');
            if (!btn || btn.disabled) return;
            if (btn.dataset.pointerHandled === '1') {
                delete btn.dataset.pointerHandled;
                return;
            }
            restock(btn);
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
                <td>${esc(it.displayName || it.name || it.id)}</td>
                <td>${it.quantity}</td>
                <td>${this.#restockButton(it)}</td>
            </tr>`).join('');
    }

    #restockButton(item) {
        const amount = item.restockAmount || 5;
        return `<button class="small restock" data-restock-item="${esc(item.id)}">+${amount}</button>`;
    }

    #productRows(products) {
        if (products.length === 0) {
            return '<tr><td colspan="2" style="color:#666">(none yet)</td></tr>';
        }

        return products.map(it => `
            <tr>
                <td>${esc(it.displayName || it.name || it.id)}</td>
                <td>${it.quantity}</td>
            </tr>`).join('');
    }
}
