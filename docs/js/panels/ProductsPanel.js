// =============================================================================
// ProductsPanel — beverage catalog. (Second-from-right column.)
//
// Product buttons are built from the controller's catalog snapshot. Clicking
// a product asks the controller to enqueue on the shortest-queue line.
// =============================================================================

import { UIComponent } from '../UIComponent.js';
import { esc, vecToArray } from '../util.js';

export class ProductsPanel extends UIComponent {
    #ctrl;

    constructor(controller) {
        super();
        this.#ctrl = controller;
    }

    bind() {
        this.refresh();

        const list = document.querySelector('#products .product-list');
        list.addEventListener('click', e => {
            const btn = e.target.closest('button[data-product-id]');
            if (!btn) return;

            const productId = btn.dataset.productId;
            const lineId = this.#ctrl.enqueueAutoProductById(productId);
            console.log('[gactorio] enqueueAutoProductById', productId, '→ line', lineId);
        });
    }

    refresh() {
        const list = document.querySelector('#products .product-list');
        const products = vecToArray(this.#ctrl.products());
        list.innerHTML = products.map(product => `
            <button class="product-btn" data-act="enqueueAuto" data-product-id="${product.id}">
                <b>${esc(product.displayName || product.name || product.id)}</b>
                <span>${product.durationSeconds.toFixed(0)} s</span>
                <span class="requirements">Uses: ${esc(product.requirements)}</span>
            </button>`).join('');
    }

    // Static panel — nothing changes per tick.
    render(_snap) {}
}
