// =============================================================================
// EventLogPanel — Observer 패턴의 이벤트 스트림을 최신순으로 표시 (좌측 컬럼 하단)
// -----------------------------------------------------------------------------
// 컨트롤러가 필요 없는 순수 표시용 패널이라 생성자/ bind() 가 없다(render 만 override).
// 스냅샷의 events 배열은 C++ EventLogObserver 가 누적한 이벤트들이 DTO 로 넘어온 것.
// =============================================================================

import { UIComponent } from '../UIComponent.js';
import { esc } from '../util.js';

export class EventLogPanel extends UIComponent {
    // 스냅샷의 이벤트를 뒤(최신)에서부터 앞으로 순회해 위에 최신이 오도록 렌더.
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
