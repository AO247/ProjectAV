//#include "BindableCommon.h"
//#include "Component.h"
//#include "Node.h"
//
//#pragma once
//class WindTunnelEffect : public Component {
//public:
//    WindTunnelEffect(Node* owner, std::shared_ptr<Bind::PixelConstantBuffer<TimeBuffer>> pCbTime)
//        : Component(owner), pCbTime(pCbTime) {
//    }
//    void Update(float dt, Graphics& gfx) {
//        float time = dt;
//        TimeBuffer tb = { time, {0,0,0} };
//        pCbTime->Update(gfx, tb);
//    }
//private:
//    std::shared_ptr<Bind::PixelConstantBuffer<TimeBuffer>> pCbTime;
//};