foo () {
    bar () {
        echo popo
        baz () {
            echo toto
        }
    }
}

bar
baz
foo
baz
bar
baz
