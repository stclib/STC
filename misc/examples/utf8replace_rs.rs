
pub fn main() {
    let mut hello = String::from("hell😀 world");
    println!("{}", hello);
    
    hello.replace_range(
        hello
            .char_indices()
            .nth(4)
            .map(|(pos, ch)| (pos..pos + ch.len_utf8()))
            .unwrap(),
        "🐨",
    );
    println!("{}", hello);

    for c in hello.chars() { 
        print!("{},", c);
    }
}
