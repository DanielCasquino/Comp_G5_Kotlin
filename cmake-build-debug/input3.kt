fun main() {
    var x: Int

    x = 1
    
    for (i in 0..9) { //tambien declara i como variable local
        x = x + i
    }
    
    println(x)
}
