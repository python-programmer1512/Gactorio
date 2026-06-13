// =============================================================================
// InventoryPanel — 원자재와 완제품 재고 표시 + 원자재 보충(+5) (오른쪽 컬럼)
// -----------------------------------------------------------------------------
// 스냅샷의 inventory 배열을 isProduct 플래그로 갈라 "원자재" / "완제품" 두 표로 그린다.
// 원자재 행의 +5 버튼은 컨트롤러 restockItem 을 호출한다(제품은 보충 버튼 없음).
// =============================================================================

import { UIComponent } from '../UIComponent.js';
import { esc } from '../util.js';

export class InventoryPanel extends UIComponent {
    #ctrl;   // Module.Controller (restockItem 명령)

    constructor(controller) {
        super();
        this.#ctrl = controller;
    }

    // bind(): 보충 버튼 클릭 위임. 매 렌더마다 표가 새로 그려지므로 위임 리스너를 쓴다.
    bind() {
        document.getElementById('inventory-content').addEventListener('pointerdown', e => {
            const btn = e.target.closest('button[data-restock-item]');
            if (!btn || btn.disabled) return;

            const itemId = parseInt(btn.dataset.restockItem, 10);
            console.log('[gactorio] restockItem', itemId, '→', this.#ctrl.restockItem(itemId));
        });
    }

    // render(): 재고를 원자재/완제품으로 분리해 두 개의 표로 출력.
    render(snap) {
        const rawItems = snap.inventory.filter(it => !it.isProduct);   // 원자재
        const products = snap.inventory.filter(it => it.isProduct);    // 완제품

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

    // 원자재 행: 이름/수량 + 보충(+5) 버튼. 비어 있으면 안내 행.
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

    // 완제품 행: 이름/수량(보충 버튼 없음). 아직 없으면 안내 행.
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
