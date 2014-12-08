#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mosquitto.h>
#include <mosquitto_plugin.h>

static char* _password;
static char* _username;

int auth_plug_check_topic(const char *topic) {
	if (topic == NULL)
	{
		return 0;
	}

	int ret_val = 0;

	int slash_count = 0;
	char *token = NULL;

	char *tokens = strdup(topic);   

	if (tokens == NULL) {
		return 0;
	}

	while (((token = strsep(&tokens, "/")) != NULL) && (slash_count <= 4)) {
		if ((slash_count == 0) && !(strncmp(token, "trails-live", 15) != 0)) {
			slash_count++;
		} else if ((slash_count == 1) && (strnlen(token, 37) == 36)) {
			slash_count++;
		} else if ((slash_count == 2) && (strncmp(token, "c", 2)) == 0) {
			slash_count++;
		} else if (slash_count == 3) {
			slash_count++;
		} else if ((slash_count == 4) &&
					((strncmp(token, "loc", 4) == 0) ||
					(strncmp(token, "info", 5) == 0) || 
					(strncmp(token, "status", 7) == 0))) {
			ret_val =  1;
		} else {
			break;
		}
	}

	return ret_val;
}

int mosquitto_auth_plugin_version(void)
{
	return MOSQ_AUTH_PLUGIN_VERSION;
}

int mosquitto_auth_plugin_init(void **user_data, struct mosquitto_auth_opt *auth_opts, int auth_opt_count)
{
	struct mosquitto_auth_opt *o;
	int i = 0;

	for (i = 0, o = auth_opts; i < auth_opt_count; i++, o++) {
		if (!strcmp(o->key, "username")) {
			_username = strdup(o->value);
		} else if (!strcmp(o->key, "password")) {
			_password = strdup(o->value);
		}
	}
	
	return MOSQ_ERR_SUCCESS;
}

int mosquitto_auth_plugin_cleanup(void *user_data, struct mosquitto_auth_opt *auth_opts, int auth_opt_count)
{
	if (_password) {
		free(_password);
	}

	if (_username) {
		free(_username);
	}

	return MOSQ_ERR_SUCCESS;
}

int mosquitto_auth_security_init(void *user_data, struct mosquitto_auth_opt *auth_opts, int auth_opt_count, bool reload)
{
	return MOSQ_ERR_SUCCESS;
}

int mosquitto_auth_security_cleanup(void *user_data, struct mosquitto_auth_opt *auth_opts, int auth_opt_count, bool reload)
{
	return MOSQ_ERR_SUCCESS;
}

int mosquitto_auth_acl_check(void *user_data, const char *clientid, const char *username, const char *topic, int access)
{
	if (username && !strncmp(username, "superuser", 200)) {
		return MOSQ_ERR_SUCCESS;
	} else if (auth_plug_check_topic(topic)) {
		return MOSQ_ERR_SUCCESS;
	} else {
		return MOSQ_ERR_ACL_DENIED;
	}
}

int mosquitto_auth_unpwd_check(void *user_data, const char *username, const char *password)
{
	if(username && !strncmp(username, "superuser", 200) && password && !strncmp(password, "superpassword", 200)){
		return MOSQ_ERR_SUCCESS;
	}

	return MOSQ_ERR_ACL_DENIED;
}

int mosquitto_auth_psk_key_get(void *user_data, const char *hint, const char *identity, char *key, int max_key_len)
{
	return MOSQ_ERR_AUTH;
}

