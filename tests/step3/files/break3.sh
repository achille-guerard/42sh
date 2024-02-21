for i in 1 2 3
    do for i in 1 2 3
        do for i in 1 2 3
            do echo child2; break 3
            done
        echo child1
        done
    echo parent
    done
