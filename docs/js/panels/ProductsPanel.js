// =============================================================================
// ProductsPanel — 음료 카탈로그 패널 (오른쪽에서 두 번째 컬럼)
// -----------------------------------------------------------------------------
// 제품 버튼을 컨트롤러의 카탈로그(products())에서 만들어 깔고, 버튼을 누르면 큐가 가장
// 짧은 라인에 그 제품을 enqueue 하도록 컨트롤러에 요청한다.
// 카탈로그는 런타임에 안 바뀌므로 bind() 때 한 번만 읽어 버튼을 생성한다(정적 패널).
// =============================================================================

import { UIComponent } from '../UIComponent.js';
import { esc, vecToArray } from '../util.js';

export class ProductsPanel extends UIComponent {
    #ctrl;   // Module.Controller (카탈로그 조회 + enqueue 명령)

    constructor(controller) {
        super();
        this.#ctrl = controller;
    }

    // bind(): 카탈로그를 읽어 제품 버튼들을 생성하고, 클릭 시 자동 enqueue 연결.
    bind() {
        const list = document.querySelector('#products .product-list');
        // products() 는 embind vector → 배열로 변환(핸들 정리 포함).
        const products = vecToArray(this.#ctrl.products());
        list.innerHTML = products.map(product => `
            <button class="product-btn" data-act="enqueueAuto" data-product-id="${product.id}">
                <b>${esc(product.name)}</b>
                <span>${product.durationSeconds.toFixed(0)} s</span>
                <span class="requirements">Uses: ${esc(product.requirements)}</span>
            </button>`).join('');

        // 클릭 위임: 어떤 제품 버튼이든 product-id 를 읽어 enqueueAutoProduct 호출.
        list.addEventListener('click', e => {
            const btn = e.target.closest('button[data-product-id]');
            if (!btn) return;

            const productId = parseInt(btn.dataset.productId, 10);
            const lineId = this.#ctrl.enqueueAutoProduct(productId);   // 가장 짧은 라인에 배정
            console.log('[gactorio] enqueueAutoProduct', productId, '→ line', lineId);
        });
    }

    // 정적 패널 — 틱마다 바뀌는 내용이 없어 render 는 비어 있음.
    render(_snap) {}
}
