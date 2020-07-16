@load frameworks/communication/listen

# Let's make sure we use the same port no matter whether we use encryption or not:
redef Communication::listen_port = 47758/tcp;

# Redef this to T if you want to use SSL.
redef Communication::listen_ssl = F;

# Set the SSL certificates being used to something real if you are using encryption.
#redef ssl_ca_certificate   = "<path>/ca_cert.pem";
#redef ssl_private_key      = "<path>/bro.pem";

redef Communication::nodes += {
    ["broconn"] = [$host = 127.0.0.1, $events = /count_update/, $connect=F, $ssl=F]
};

global connection_count : count;

function services_to_string(ss: string_set): string
{
    local result = "";

    for (s in ss)
        result = fmt("%s %s", result, s);

    return result;
}

global send_count: event( cc: count);
global count_update: event( );

event bro_init () {
	connection_count = 0;
}

event new_connection(c: connection)
{
    print fmt("new_connection: %s, services:%s",
              id_string(c$id), services_to_string(c$service));
   connection_count += 1;
   event send_count (connection_count);
}

event send_count( cc: count ) {
	print fmt ("Connection Count %d", cc );
}


event count_update() {
	print fmt ("Recieved count_update event" );
}
