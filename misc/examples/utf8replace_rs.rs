pub fn main() {
    let mut hello = String::from("hell😀 w😀rld");
    println!("{}", hello);

    /* replace second smiley at utf8 codepoint pos 7 */
    hello.replace_range(
        hello
            .char_indices()
            .nth(7)
            .map(|(pos, ch)| (pos..pos + ch.len_utf8()))
            .unwrap(),
        "🐨",
    );
    println!("{}", hello);

    for c in hello.chars() { 
        print!("{},", c);
    }

    let str = "If you find the time, you will find the winner";
    println!("\n{}", str.replace("find", "match"));
}
