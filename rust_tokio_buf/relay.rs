use tokio::io;
use tokio::net::{TcpListener, TcpStream};

use futures::future::try_join;
use futures::FutureExt;
use std::env;
use std::error::Error;

#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    let listen_addr = env::args().nth(1).unwrap_or("127.0.0.1:1090".to_string());
    let server_addr = env::args().nth(2).unwrap_or("127.0.0.1:5201".to_string());

    println!("Listening on: {}", listen_addr);
    println!("Proxying to: {}", server_addr);

    let mut listener = TcpListener::bind(listen_addr).await?;

    while let Ok((inbound, _)) = listener.accept().await {
        let transfer = transfer(inbound, server_addr.clone()).map(|r| {
            if let Err(e) = r {
                println!("Failed to transfer; error={}", e);
            }
        });

        tokio::spawn(transfer);
    }

    Ok(())
}

async fn transfer(mut inbound: TcpStream, proxy_addr: String) -> Result<(), Box<dyn Error>> {
    let mut outbound = TcpStream::connect(proxy_addr).await?;

    let (ri, wi) = inbound.split();
    let (ro, wo) = outbound.split();

    let mut ri2 = tokio::io::BufReader::new(ri);
    let mut ro2 = tokio::io::BufReader::new(ro);

    let mut wi2 = tokio::io::BufWriter::new(wi);
    let mut wo2 = tokio::io::BufWriter::new(wo);

    let client_to_server = io::copy(&mut ri2, &mut wo2);
    let server_to_client = io::copy(&mut ro2, &mut wi2);

    try_join(client_to_server, server_to_client).await?;

    Ok(())
}
