#pragma once

namespace UFT
{
    class FastTravelEventHandler :
        public BSTEventSink <TESFastTravelEndEvent>
    {
    protected:
        virtual EventResult	ReceiveEvent(TESFastTravelEndEvent* evn, EventDispatcher<TESFastTravelEndEvent>* dispatcher) override;
    };

    struct PFTState {
        std::atomic<bool> over_encumbered;
        std::atomic<bool> guards_pursuing;
        std::atomic<bool> combat;
        std::atomic<bool> taking_damage;
        std::atomic<bool> location;
        std::atomic<bool> in_air;
        std::atomic<bool> worldspace_travel;
        std::atomic<bool> script_cond;
        std::atomic<bool> vamp_feed;
    };

    extern PFTState pft_state;
    extern void Initialize();
}