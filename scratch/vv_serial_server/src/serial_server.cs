using System;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;

public class SynchronousSocketListener {
	public static void StartListening( ) {
		byte[] bytes = new Byte[1024];
		bool keep_alive = true;

		IPHostEntry ipHostInfo = Dns.GetHostEntry( Dns.GetHostName() );
		IPAddress ipAddress = ipHostInfo.AddressList[0];
		IPEndPoint localEndPoint = new IPEndPoint( ipAddress, 10100 );

		Socket listener = new Socket( ipAddress.AddressFamily, SocketType.Stream, ProtocolType.Tcp );

		try {
			listener.Bind( localEndPoint );
			listener.Listen( 3 );

			Console.WriteLine( "Server active" );

			while( true ) {
				Console.WriteLine( "Waiting for connection..." );
				// Program is suspended while waiting for an incoming connection.  
				Socket handler = listener.Accept();

				Console.WriteLine( "Connection established" );  
				
				keep_alive = true;

				while( keep_alive ) {
					string data = null;

					do {
						int bytesRec = handler.Receive( bytes );
						data += Encoding.ASCII.GetString( bytes, 0, bytesRec );
					}  while( data.IndexOf( (char)26 ) == -1 );

					data = data.TrimEnd( (char)26 );
					byte[] msg = Encoding.ASCII.GetBytes( data );
					byte[] eof = { 26 };
					
					Console.WriteLine( "Command received: {0}", data );

					string[] cmd_parts = data.Split( ' ' );

					if( cmd_parts[0].Equals("READ") ) {
						msg = File.ReadAllBytes( cmd_parts[1] );
						//Console.WriteLine( "Sending: {0}", Encoding.ASCII.GetString( msg ) );
						Console.WriteLine( "Sent!" );
					}

					if( data.Equals("vv:close-connection") ) {
						keep_alive = false;
					}

					handler.Send( msg );
					handler.Send( eof );
				}

				handler.Shutdown( SocketShutdown.Both );
				handler.Close();
			}

		} catch( Exception e ) {
			Console.WriteLine( e.ToString() );
		}
	}

	public static int Main( String[] args ) {
		StartListening();
		return 0;
	}
}