// =============================================================================
// ProductsPanel — beverage catalog. (Second-from-right column.)
//
// The three product buttons are static HTML in index.html, so plain 'click'
// handlers are safe here (only re-rendered DOM needs pointerdown). Clicking
// a product asks the controller to enqueue on the shortest-queue line.
// =============================================================================

import { UIComponent } from '../UIComponent.js';

export class ProductsPanel extends UIComponent {
    #ctrl;

    constructor(controller) {
        super();
        this.#ctrl = controller;
    }

    bind() {
        for (const btn of document.querySelectorAll('#products .product-btn')) {
            btn.addEventListener('click', () => {
                const kindName = btn.dataset.kind;
                const kindEnum = Module.ProductKind[kindName];
                if (kindEnum === undefined) {
                    console.error('[gactorio] unknown ProductKind:', kindName);
                    return;
                }
                const lineId = this.#ctrl.enqueueAuto(kindEnum);
                console.log('[gactorio] enqueueAuto', kindName, '→ line', lineId);
            });
        }
    }

    // Static panel — nothing changes per tick.
    render(_snap) {}
}
