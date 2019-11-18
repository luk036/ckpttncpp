
.. _program_listing_file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_ckpttncpp_FMPartMgr.hpp:

Program Listing for File FMPartMgr.hpp
======================================

|exhale_lsh| :ref:`Return to documentation for file <file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_ckpttncpp_FMPartMgr.hpp>` (``/media/lubuntu/USBDISK/github/ckpttncpp/lib/include/ckpttncpp/FMPartMgr.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   // **Special code for two-pin nets**
   // Take a snapshot when a move make **negative** gain.
   // Snapshot in the form of "interface"???
   #include "PartMgrBase.hpp"
   #include <gsl/span>
   
   template <typename GainMgr, typename ConstrMgr> //
   class FMPartMgr : public PartMgrBase<GainMgr, ConstrMgr, FMPartMgr>
   {
       using Base = PartMgrBase<GainMgr, ConstrMgr, FMPartMgr>;
   
     public:
       FMPartMgr(SimpleNetlist& H, GainMgr& gainMgr, ConstrMgr& constrMgr)
           : Base {H, gainMgr, constrMgr}
       {
       }
   
       auto take_snapshot(gsl::span<const uint8_t> part) -> std::vector<uint8_t>
       {
           auto snapshot = std::vector<uint8_t> {};
           const auto N = part.size();
           snapshot.reserve(N);
           for (auto i = 0U; i < N; ++i)
           {
               snapshot[i] = part[i];
           }
           return snapshot;
       }
   
       auto restore_part(std::vector<uint8_t>& snapshot, gsl::span<uint8_t> part)
           -> void
       {
           const auto N = part.size();
           for (auto i = 0U; i < N; ++i)
           {
               part[i] = snapshot[i];
           }
       }
   };
