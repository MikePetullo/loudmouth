/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2004 Imendio AB
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Small tool to register an account
 */

#include <loudmouth/loudmouth.h>

static void
print_usage (const gchar *exec_name)
{
        g_print ("Usage: %s <server> <username> <password>\n",
                 exec_name);
}

int
main (int argc, char **argv)
{
        LmConnection  *connection;
        const gchar   *server;
        const gchar   *username;
        const gchar   *pass;
        GError        *error = NULL;
        LmMessage     *m, *reply;
        LmMessageNode *query, *node;

        if (argc < 4) {
                print_usage (argv[0]);
                return -1;
        }

        server = argv[1];
        username = argv[2];
        pass = argv[3];

        connection = lm_connection_new (server);

        if (!lm_connection_open_and_block (connection, &error)) {
                g_error ("Failed to open: %s\n", error->message);
        }

	m = lm_message_new_with_sub_type (NULL, LM_MESSAGE_TYPE_IQ,
					  LM_MESSAGE_SUB_TYPE_SET);

        query = lm_message_node_add_child (m->node, "query", NULL);

        lm_message_node_set_attributes (query, "xmlns", "jabber:iq:register",
                                        NULL);
        lm_message_node_add_child (query, "username", username);
        lm_message_node_add_child (query, "password", pass);

	reply = lm_connection_send_with_reply_and_block (connection, 
							 m, &error);
	
        if (!reply) {
                g_error ("Failed to send registration request: %s\n", 
			 error->message);
        }

	switch (lm_message_get_sub_type (reply)) {
	case LM_MESSAGE_SUB_TYPE_RESULT:
		g_print ("Succeeded in register account '%s@%s'\n",
			 username, server);
		break;
	case LM_MESSAGE_SUB_TYPE_ERROR:
	default:
		g_print ("Failed to register account '%s@%s' due to: ",
			 username, server);
		
		node = lm_message_node_find_child (reply->node, "error");
		if (node) {
			g_print ("%s\n", lm_message_node_get_value (node));
		} else {
			g_print ("Unknown error\n");
		}
		break;
	}

        lm_connection_close (connection, NULL);

	return 0;
}

