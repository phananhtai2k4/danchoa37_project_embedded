#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include <touchgfx/widgets/canvas/Circle.hpp>

class Screen1View : public Screen1ViewBase
{
public:
    Screen1View();
    virtual ~Screen1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    void tickEvent();
    protected:
        uint32_t tickCount = 0;
    private: // Hoặc protected
        // Circle circle1; // Đã được kế thừa từ Screen1ViewBase nếu bạn thêm nó trong TouchGFX Designer

        float currentAngleX;
        float currentAngleY; // Để tính delta time chính xác hơn nếu cần
        int16_t initialBallX;
        int16_t initialBallY;
};

#endif // SCREEN1VIEW_HPP
