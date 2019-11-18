
.. _program_listing_file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_ckpttncpp_FMKWayGainMgr.hpp:

Program Listing for File FMKWayGainMgr.hpp
==========================================

|exhale_lsh| :ref:`Return to documentation for file <file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_ckpttncpp_FMKWayGainMgr.hpp>` (``/media/lubuntu/USBDISK/github/ckpttncpp/lib/include/ckpttncpp/FMKWayGainMgr.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "FMGainMgr.hpp"
   #include "FMKWayGainCalc.hpp"
   #include <gsl/span>
   
   class FMKWayGainMgr;
   
   class FMKWayGainMgr : public FMGainMgr<FMKWayGainCalc, FMKWayGainMgr>
   {
   
     private:
       robin<uint8_t> RR;
   
     public:
       using Base = FMGainMgr<FMKWayGainCalc, FMKWayGainMgr>;
       using GainCalc_ = FMKWayGainCalc;
       using index_t = typename SimpleNetlist::index_t;
   
       FMKWayGainMgr(SimpleNetlist& H, uint8_t K)
           : Base {H, K}
           , RR {K}
       {
       }
   
       auto init(gsl::span<const uint8_t> part) -> int;
   
       auto modify_key(node_t w, uint8_t part_w, const std::vector<int>& keys)
           -> void
       {
           for (auto k : this->RR.exclude(part_w))
           {
               this->gainbucket[k].modify_key(
                   this->gainCalc.vertex_list[k][w], keys[k]);
           }
       }
   
       auto update_move_v(const MoveInfoV& move_info_v, int gain) -> void;
   
       auto lock(index_t whichPart, node_t v) -> void
       {
           auto& vlink = this->gainCalc.vertex_list[whichPart][v];
           this->gainbucket[whichPart].detach(vlink);
           vlink.lock();
       }
   
       auto lock_all(index_t /*fromPart*/, node_t v) -> void
       {
           for (auto k = 0U; k < this->K; ++k)
           {
               this->lock(k, v);
           }
       }
   
     private:
       auto __set_key(index_t whichPart, node_t v, int key) -> void
       {
           this->gainbucket[whichPart].set_key(
               this->gainCalc.vertex_list[whichPart][v], key);
       }
   };
