# FE JS View Layout Update

## 변경 요약

현재 View 배치는 다음 구조를 사용한다.

```text
Simulation Control | Factory Floor | Inspector | Products + Inventory
```

`Products`와 `Inventory`는 별도 컬럼이 아니라 `#catalog-stack`이라는 하나의 DOM 영역 안에서 위아래로 배치된다.

```text
#catalog-stack
  ├─ ProductsPanel 영역
  └─ InventoryPanel 영역
```

`Inspector`는 `Products/Inventory` 묶음과 분리된 독립 영역을 차지한다. 따라서 선택된 기계의 상세 상태와 조작 버튼은 한 패널에서 유지되고, 제품 투입과 재고 보충은 오른쪽의 compact catalog 영역에서 처리된다.

## UML 수정 여부

FE JS View class UML은 수정하지 않아도 된다.

이유는 이번 변경이 class dependency 변경이 아니라 HTML/CSS layout 변경이기 때문이다.

- `ProductsPanel`은 여전히 `UIComponent`를 상속하고 `Module.Controller`에 command를 보낸다.
- `InventoryPanel`은 여전히 `UIComponent`를 상속하고 `Module.Controller`에 restock command를 보낸다.
- `InspectorPanel`은 여전히 `UIComponent`를 상속하고 선택된 `MachineView` snapshot을 표시한다.
- `AppUI`가 panel 목록을 소유하고 `renderAll(snapshot)`을 호출하는 구조도 그대로다.

따라서 UML class diagram의 inheritance, dependency, association 화살표는 변경되지 않는다.

## 보고서/발표 설명

보고서에는 다음과 같이 설명하면 된다.

> The JavaScript View keeps the same MVC panel structure, but the CSS layout was refined for usability. Inspector is placed as an independent side panel for selected-machine operations. Products and Inventory share a compact catalog stack, because both panels are auxiliary controls and do not need independent full-height columns. This layout change does not alter the UML class relationships because the panel classes and controller dependencies remain unchanged.

## 평가 기준 관점

- MVC: 유지됨. View는 여전히 `Module.Controller`만 호출한다.
- SRP: 유지됨. `InspectorPanel`, `ProductsPanel`, `InventoryPanel`의 책임은 합쳐지지 않았다.
- OCP: 유지됨. 배치는 CSS/HTML에서 바뀌었고 panel class 추가/삭제 없이 확장 가능하다.
- UML: class 관계 변화가 없으므로 drawio class diagram 변경은 불필요하다.
