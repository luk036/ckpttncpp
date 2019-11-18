
.. _program_listing_file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_ckpttncpp_FMGainMgr.hpp:

Program Listing for File FMGainMgr.hpp
======================================

|exhale_lsh| :ref:`Return to documentation for file <file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_ckpttncpp_FMGainMgr.hpp>` (``/media/lubuntu/USBDISK/github/ckpttncpp/lib/include/ckpttncpp/FMGainMgr.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "bpqueue.hpp" // import bpqueue
   #include "dllist.hpp"  // import dllink
   #include "netlist.hpp" // import Netlist
   #include <cinttypes>
   #include <gsl/span>
   #include <iterator>
   #include <memory>
   #include <type_traits>
   
   template <typename GainCalc, class Derived>
   class FMGainMgr
   {
       Derived& self = *static_cast<Derived*>(this);
       using index_t = typename SimpleNetlist::index_t;
   
     protected:
       dllink<node_t> waitinglist {};
   
       SimpleNetlist& H;
       GainCalc gainCalc;
       size_t pmax;
       std::vector<bpqueue<int>> gainbucket;
   
     public:
       uint8_t K;
       // int totalcost;
   
       explicit FMGainMgr(SimpleNetlist& H, uint8_t K);
   
       auto init(gsl::span<const uint8_t> part) -> int;
   
       [[nodiscard]] auto is_empty_togo(uint8_t toPart) const -> bool
       {
           return this->gainbucket[toPart].is_empty();
       }
   
       [[nodiscard]] auto is_empty() const -> bool
       {
           for (auto k = 0U; k < this->K; ++k)
           {
               if (!this->gainbucket[k].is_empty())
               {
                   return false;
               }
           }
           return true;
       }
   
       auto select(gsl::span<const uint8_t> part) -> std::tuple<MoveInfoV, int>;
   
       auto select_togo(uint8_t toPart) -> std::tuple<index_t, int>;
   
       auto update_move(
           gsl::span<const uint8_t> part, const MoveInfoV& move_info_v) -> void;
   
     private:
       auto __update_move_2pin_net(
           gsl::span<const uint8_t> part, const MoveInfo& move_info) -> void;
   
       auto __update_move_3pin_net(
           gsl::span<const uint8_t> part, const MoveInfo& move_info) -> void;
   
       auto __update_move_general_net(
           gsl::span<const uint8_t> part, const MoveInfo& move_info) -> void;
   };
