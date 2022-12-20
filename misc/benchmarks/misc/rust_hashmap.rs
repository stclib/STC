use std::{
    hash::{BuildHasherDefault, Hasher},
    io::Read,
    time::Instant,
};

struct MyHasher {
    seed: u64,
}

impl Default for MyHasher {
    fn default() -> Self {
        Self { seed: 0xb5ad4eceda1ce2a9_u64 }
    }
}

impl Hasher for MyHasher {
    fn write(&mut self, bytes: &[u8]) {
        use std::convert::TryInto;
        self.seed = u64::from_ne_bytes(bytes.try_into().unwrap()).wrapping_mul(0xc6a4a7935bd1e99d);
    }

    #[inline]
    fn write_u64(&mut self, i: u64) {
        self.seed = i.wrapping_mul(0xc6a4a7935bd1e99d);
    }

    #[inline]
    fn finish(&self) -> u64 {
        self.seed
    }
}

type MyBuildHasher = BuildHasherDefault<MyHasher>;

fn romu_trio(s: &mut [u64]) -> u64 {
    let xp = s[0];
    let yp = s[1];
    let zp = s[2];
    s[0] = 15241094284759029579_u64.wrapping_mul(zp);
    s[1] = yp.wrapping_sub(xp);
    s[1] = s[1].rotate_left(12);
    s[2] = zp.wrapping_sub(yp);
    s[2] = s[2].rotate_left(44);
    return xp;
}

fn main() {
    let n = 50_000_000;
    let mask = (1 << 25) - 1;

    let mut m = std::collections::HashMap::<u64, u64, MyBuildHasher>::default();
    m.reserve(n);

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
    println!("press a key:");
    std::io::stdin().bytes().next();
}