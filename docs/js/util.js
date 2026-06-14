// =============================================================================
// util.js — View 계층 공용 헬퍼
// 핵심: embind 핸들 처리. C++ 에서 넘어온 vector 핸들은 GC 대상이 아니라 직접
//       delete() 해야 메모리 누수가 없다(여기서 일괄 처리해 패널 코드를 단순하게).
// =============================================================================

// innerHTML 에 넣기 전 신뢰할 수 없는 문자열을 이스케이프(XSS/깨짐 방지).
export function esc(s) {
    return String(s)
        .replaceAll('&', '&amp;')
        .replaceAll('<', '&lt;')
        .replaceAll('>', '&gt;')
        .replaceAll('"', '&quot;');
}

// embind vector 핸들 → 일반 JS 배열로 변환하고, 그 후 C++ 쪽 할당을 해제(delete).
// embind 핸들은 자바스크립트 GC 가 회수하지 않는다 — delete() 를 빼먹으면 10Hz 렌더마다
// 힙 블록이 조금씩 새어나간다(예전 단일 app.js 의 실제 버그였음).
export function vecToArray(vec) {
    const out = [];
    for (let i = 0; i < vec.size(); i++) out.push(vec.get(i));
    vec.delete();   // C++ 메모리 해제(필수)
    return out;
}

// Module.Controller.snapshot() 결과(embind 객체)를 순수 데이터로 평탄화한다.
// 이 호출 이후엔 살아있는 embind 핸들이 없으므로, 패널들은 스냅샷을 그냥 평범한 JS
// 객체처럼 다루면 되고 별도 정리(cleanup)가 필요 없다.
export function toPlainSnapshot(snap) {
    return {
        simulationTime: snap.simulationTime,
        stats:          snap.stats,
        lines: vecToArray(snap.lines).map(line => ({
            ...line,
            machines: vecToArray(line.machines),   // 중첩 vector 도 배열로
        })),
        events:    vecToArray(snap.events),
        inventory: vecToArray(snap.inventory),
    };
}
