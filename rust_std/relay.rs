use std::error::Error;
use std::io::prelude::*;
use std::thread;

fn main() -> Result<(), Box<dyn Error>> {
    let listen_addr = "127.0.0.1:1090".to_string();
    let server_addr = "127.0.0.1:5201".to_string();

    println!("Listening on: {}", listen_addr);
    println!("Proxying to: {}", server_addr);

    let listener = std::net::TcpListener::bind(listen_addr)?;

    while let Ok((inbound, _)) = listener.accept() {
        thread::spawn(|| {
            let server_addr = "127.0.0.1:5201".to_string();
            let _ = transfer(inbound, &server_addr);
        });
    }

    Ok(())
}

fn transfer(mut inbound: std::net::TcpStream, proxy_addr: &String) -> Result<(), Box<dyn Error>> {
    let mut outbound = std::net::TcpStream::connect(proxy_addr)?;

    let mut buf = [0; 130 * 1024];
    loop {
        let size = inbound.read(&mut buf)?;
        if size == 0 {
            break;
        }

        let _ = outbound.write(&mut buf);
    }

    Ok(())
}
