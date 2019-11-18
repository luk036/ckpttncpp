
.. _program_listing_file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_ckpttncpp_FMBiGainMgr.hpp:

Program Listing for File FMBiGainMgr.hpp
========================================

|exhale_lsh| :ref:`Return to documentation for file <file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_ckpttncpp_FMBiGainMgr.hpp>` (``/media/lubuntu/USBDISK/github/ckpttncpp/lib/include/ckpttncpp/FMBiGainMgr.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "FMBiGainCalc.hpp"
   #include "FMGainMgr.hpp"
   #include <gsl/span>
   
   struct FMBiGainMgr;
   
   struct FMBiGainMgr : public FMGainMgr<FMBiGainCalc, FMBiGainMgr>
   {
       using Base = FMGainMgr<FMBiGainCalc, FMBiGainMgr>;
       using GainCalc_ = FMBiGainCalc;
       using index_t = typename SimpleNetlist::index_t;
   
       explicit FMBiGainMgr(SimpleNetlist& H)
           : Base {H, 2}
       {
       }
   
       FMBiGainMgr(SimpleNetlist& H, uint8_t /* K */)
           : Base {H, 2}
       {
       }
   
       auto init(gsl::span<const uint8_t> part) -> int;
   
       auto modify_key(node_t w, uint8_t part_w, int key) -> void
       {
           this->gainbucket[1 - part_w].modify_key(
               this->gainCalc.vertex_list[w], key);
       }
   
       auto update_move_v(const MoveInfoV& move_info_v, int gain) -> void
       {
           // this->vertex_list[v].key -= 2 * gain;
           // auto [fromPart, _ = move_info_v;
           this->__set_key(move_info_v.fromPart, move_info_v.v, -gain);
       }
   
       auto lock(uint8_t whichPart, node_t v) -> void
       {
           auto& vlink = this->gainCalc.vertex_list[v];
           this->gainbucket[whichPart].detach(vlink);
           vlink.lock();
       }
   
       auto lock_all(uint8_t fromPart, node_t v) -> void
       {
           auto toPart = 1 - fromPart;
           this->lock(toPart, v);
       }
   
     private:
       auto __set_key(uint8_t whichPart, node_t v, int key) -> void
       {
           this->gainbucket[whichPart].set_key(this->gainCalc.vertex_list[v], key);
       }
   };
