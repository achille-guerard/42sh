for i in 1 2 3
    do for i in 1 2 3
        do for i in 1 2 3
            do echo child2; continue 2; echo prout
            done
        echo child1
        done
    echo parent
    done
