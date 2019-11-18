
.. _program_listing_file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_ckpttncpp_FMKWayGainCalc.hpp:

Program Listing for File FMKWayGainCalc.hpp
===========================================

|exhale_lsh| :ref:`Return to documentation for file <file__media_lubuntu_USBDISK_github_ckpttncpp_lib_include_ckpttncpp_FMKWayGainCalc.hpp>` (``/media/lubuntu/USBDISK/github/ckpttncpp/lib/include/ckpttncpp/FMKWayGainCalc.hpp``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS

.. code-block:: cpp

   #pragma once
   
   #include "dllist.hpp"  // import dllink
   #include "netlist.hpp" // import Netlist
   #include "robin.hpp"   // import robin
   #include <gsl/span>
   
   class FMKWayGainMgr;
   
   class FMKWayGainCalc
   {
       friend class FMKWayGainMgr;
       using index_t = typename SimpleNetlist::index_t;
   
     private:
       SimpleNetlist& H;
       uint8_t K;
       robin<uint8_t> RR;
       size_t num_modules;
       std::vector<std::vector<dllink<node_t>>> vertex_list;
       std::vector<int> deltaGainV;
   
     public:
       int totalcost {0};
   
       FMKWayGainCalc(SimpleNetlist& H, uint8_t K)
           : H {H}
           , K {K}
           , RR {K}
           , num_modules {H.number_of_modules()}
           , deltaGainV(K, 0)
       {
           for (auto k = 0U; k < this->K; ++k)
           {
               this->vertex_list.emplace_back(
                   std::vector<dllink<node_t>>(this->num_modules));
           }
       }
   
       auto start_ptr(uint8_t toPart) -> dllink<node_t>*
       {
           return &this->vertex_list[toPart][0];
       }
   
       auto init(gsl::span<const uint8_t> part) -> int
       {
           this->totalcost = 0;
           for (auto k = 0U; k < this->K; ++k)
           {
               for (auto& vlink : this->vertex_list[k])
               {
                   vlink.key = 0;
               }
           }
           for (auto net : this->H.nets)
           {
               this->__init_gain(net, part);
           }
           return this->totalcost;
       }
   
       auto update_move_init() -> void
       {
           std::fill_n(this->deltaGainV.begin(), this->K, 0);
       }
   
       using ret_2pin_info = std::tuple<index_t, std::vector<int>>;
   
       auto update_move_2pin_net(gsl::span<const uint8_t> part,
           const MoveInfo& move_info) -> ret_2pin_info;
   
       using ret_info =
           std::tuple<std::vector<node_t>, std::vector<std::vector<int>>>;
   
       auto update_move_3pin_net(
           gsl::span<const uint8_t> part, const MoveInfo& move_info) -> ret_info;
   
       auto update_move_general_net(
           gsl::span<const uint8_t> part, const MoveInfo& move_info) -> ret_info;
   
     private:
       auto __modify_gain(node_t v, uint8_t part_v, int weight) -> void
       {
           for (auto k : this->RR.exclude(part_v))
           {
               this->vertex_list[k][v].key += weight;
           }
       }
   
       auto __init_gain(node_t net, gsl::span<const uint8_t> part) -> void;
   
       auto __init_gain_2pin_net(node_t net, gsl::span<const uint8_t> part)
           -> void;
   
       auto __init_gain_3pin_net(node_t net, gsl::span<const uint8_t> part)
           -> void;
   
       auto __init_gain_general_net(node_t net, gsl::span<const uint8_t> part)
           -> void;
   };
