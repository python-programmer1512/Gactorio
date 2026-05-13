# Energy Drink Factory Headers

에너지 음료 생산 공정 OOP 프로젝트의 C++ 헤더 파일 골격입니다.

## 핵심 구조

- `Product`: 완성 음료 객체
- `Ingredient`: 재료 객체
- `Recipe`: 음료 제조 레시피
- `Inventory`: 재료 재고 관리
- `Machine`: 공정 기계의 기본 클래스
- `Factory`: 추상 공장 클래스
- `MixingFactory`: 재료 혼합 공정
- `CarbonationFactory`: 탄산 주입 공정
- `PackagingFactory`: 포장 공정
- `ProductionLine`: 여러 공장을 순서대로 실행
- `Order`: 주문 정보

## 공장 조건

공장 클래스는 최소 3개입니다.

1. `MixingFactory`
2. `CarbonationFactory`
3. `PackagingFactory`
