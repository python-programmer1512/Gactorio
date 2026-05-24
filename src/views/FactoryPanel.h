#pragma once

#include "UIComponent.h"
#include "../controllers/FactoryController.h"

// FactoryPanel은 공장 전체를 한 윈도우로 보여주는 View 컴포넌트.
// FactoryController를 참조로 받아 데이터 조회/액션 호출. Model 직접 접근 없음.
class FactoryPanel : public UIComponent {
public:
    explicit FactoryPanel(FactoryController& ctrl);
    void render() override;

private:
    FactoryController& m_ctrl;
};
