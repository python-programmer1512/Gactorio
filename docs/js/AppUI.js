// =============================================================================
// AppUI — 패널 합성기(compositor)
// -----------------------------------------------------------------------------
// ImGui 시절 src/views/AppUI.h 의 대응물: 패널 목록을 들고 있다가 render() 를 등록된
// 모든 패널에 뿌린다. 새 패널을 추가해도 AppUI 코드는 고칠 필요가 없다(OCP) — 그냥
// addPanel() 로 등록만 하면 된다.
//
// has-a: AppUI ◆ UIComponent[]  (패널들을 합성으로 소유)
// =============================================================================

import { UIComponent } from './UIComponent.js';

export class AppUI {
    #panels = [];   // 등록된 패널 목록(private 필드)

    // 패널 등록. UIComponent 가 아니면 타입 오류. 등록 즉시 bind()로 이벤트 연결.
    addPanel(panel) {
        if (!(panel instanceof UIComponent)) {
            throw new TypeError('addPanel expects a UIComponent');
        }
        this.#panels.push(panel);
        panel.bind();
    }

    // 한 프레임 렌더: 모든 패널에 같은 스냅샷을 넘겨 render() 호출(다형성).
    renderAll(snapshot) {
        for (const panel of this.#panels) panel.render(snapshot);
    }
}
