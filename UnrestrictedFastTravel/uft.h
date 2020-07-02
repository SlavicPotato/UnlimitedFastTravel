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
        std::atomic<bool> location;
        std::atomic<bool> worldspace_travel;
    };

    extern PFTState pft_state;
    extern void Initialize();
}