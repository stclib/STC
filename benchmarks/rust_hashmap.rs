use std::time::{Instant};
use std::io::Read;

fn romu_rotl(val: u64, r: u32) -> u64
    { return (val << r) | (val >> (64 - r)); }

fn romu_trio(s: &mut[u64]) -> u64 {
    let xp = s[0]; let yp = s[1]; let zp = s[2];
    s[0] = 15241094284759029579 * zp;
    s[1] = yp - xp; s[1] = romu_rotl(s[1], 12);
    s[2] = zp - yp; s[2] = romu_rotl(s[2], 44);
    return xp;
}

fn main() {
    let n = 50_000_000; let mask = (1 << 25) - 1;
    let mut m = std::collections::HashMap::<u64, u64>::with_capacity(n);
    let mut rng: [u64; 3] = [1872361123, 123879177, 87739234];
    println!("Rust HashMap  n = {}, mask = {:#x}", n, mask);
    let now = Instant::now();
    for _i in 0..n {
        let key: u64 = romu_trio(&mut rng) & mask;
        *m.entry(key).or_insert(0) += 1;
    }
    println!("insert  : {}ms  \tsize : {}", now.elapsed().as_millis(), m.len());
    let now = Instant::now();
    let mut sum = 0;
    for i in 0..mask + 1 { if m.contains_key(&i) { sum += 1; }}
    println!("lookup  : {}ms  \tsum  : {}", now.elapsed().as_millis(), sum);

    let now = Instant::now();
    let mut sum = 0;
    for (_, value) in &m { sum += value; }
    println!("iterate : {}ms  \tsum  : {}", now.elapsed().as_millis(), sum);

    let mut rng: [u64; 3] = [1872361123, 123879177, 87739234];
    let now = Instant::now();
    for _ in 0..n {
        let key: u64 = romu_trio(&mut rng) & mask;
        m.remove(&key);
    }
    println!("remove  : {}ms  \tsize : {}", now.elapsed().as_millis(), m.len());
    println!("press a key:"); std::io::stdin().bytes().next();
}