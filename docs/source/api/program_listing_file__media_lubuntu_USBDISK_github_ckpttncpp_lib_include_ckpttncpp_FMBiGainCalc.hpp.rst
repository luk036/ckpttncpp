
.. _program_listing_file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_ckpttncpp_FMBiGainCalc.hpp:

Program Listing for File FMBiGainCalc.hpp
=========================================

|exhale_lsh| :ref:`Return to documentation for file <file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_ckpttncpp_FMBiGainCalc.hpp>` (``/media/lubuntu/USBDISK/github/ckpttncpp/lib/include/ckpttncpp/FMBiGainCalc.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "dllist.hpp"  // import dllink
   #include "netlist.hpp" // import Netlist
   #include <gsl/span>
   
   struct FMBiGainMgr;
   
   class FMBiGainCalc
   {
       friend class FMBiGainMgr;
       using index_t = typename SimpleNetlist::index_t;
   
     private:
       SimpleNetlist& H;
       size_t num_modules {};
       std::vector<dllink<node_t>> vertex_list;
   
     public:
       int totalcost;
   
       explicit FMBiGainCalc(SimpleNetlist& H, uint8_t /*K*/)
           : H {H}
           , vertex_list(H.number_of_modules())
       {
       }
   
       auto init(gsl::span<const uint8_t> part) -> int
       {
           this->totalcost = 0;
           for (auto& vlink : this->vertex_list)
           {
               vlink.key = 0;
           }
           for (auto net : this->H.nets)
           {
               this->__init_gain(net, part);
           }
           return this->totalcost;
       }
   
       auto start_ptr(uint8_t /*toPart*/) -> dllink<node_t>*
       {
           return &this->vertex_list[0];
       }
   
       auto update_move_init() -> void
       {
           // nothing to do in 2-way partitioning
       }
   
       using ret_2pin_info = std::tuple<index_t, int>;
   
       auto update_move_2pin_net(gsl::span<const uint8_t> part,
           const MoveInfo& move_info) -> ret_2pin_info;
   
       using ret_info = std::tuple<std::vector<node_t>, std::vector<int>>;
   
       auto update_move_3pin_net(
           gsl::span<const uint8_t> part, const MoveInfo& move_info) -> ret_info;
   
       auto update_move_general_net(
           gsl::span<const uint8_t> part, const MoveInfo& move_info) -> ret_info;
   
     private:
       auto __modify_gain(node_t w, int weight) -> void
       {
           this->vertex_list[w].key += weight;
       }
   
       auto __init_gain(node_t net, gsl::span<const uint8_t> part) -> void;
   
       auto __init_gain_2pin_net(node_t net, gsl::span<const uint8_t> part)
           -> void;
   
       auto __init_gain_3pin_net(node_t net, gsl::span<const uint8_t> part)
           -> void;
   
       auto __init_gain_general_net(node_t net, gsl::span<const uint8_t> part)
           -> void;
   };
