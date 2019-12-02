use async_std::io;
use async_std::net::{TcpListener, TcpStream};
use async_std::prelude::*;
use async_std::task;

fn main() -> io::Result<()> {
    println!("Listening on 127.0.0.1:1090");
    println!("Forwading to 127.0.0.1:5201");

    task::block_on(async {
        let listener = TcpListener::bind("127.0.0.1:1090").await?;

        let mut incoming = listener.incoming();

        while let Some(stream) = incoming.next().await {
            let inbound = stream?;
            task::spawn(async {
                relay(inbound).await.unwrap();
            });
        }
        Ok(())
    })
}

async fn relay(mut inbound: TcpStream) -> io::Result<()> {
    let mut outbound = TcpStream::connect("127.0.0.1:5201").await?;

    io::copy(&mut inbound, &mut outbound).await?;

    Ok(())
}
