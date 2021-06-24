# TODO

- Min-hash
- move contract_subgraph() inside HeirNetlist
- restrict weight, key (gain) to int32_t
- restrict weight unsigned


K = 2
2 buckets
1 vertex_list
1 waiting_list

general K
K buckets
K vertex_lists ???
1 waiting_list

The package range-v3:x86-windows provides CMake targets:

    find_package(range-v3 CONFIG REQUIRED)
        # Note: 2 target(s) were omitted.
	    target_link_libraries(main PRIVATE range-v3 range-v3-meta range-v3::meta range-v3-concepts)

The package doctest:x86-windows provides CMake targets:

    find_package(doctest CONFIG REQUIRED)
        target_link_libraries(main PRIVATE doctest::doctest)


# Backup

  473  sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-key C99B11DEB97541F0
  474  sudo apt-add-repository https://cli.github.com/packages
  475  sudo apt update
  476  sudo apt install gh

