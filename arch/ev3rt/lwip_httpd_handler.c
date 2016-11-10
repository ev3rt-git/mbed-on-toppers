/*
 * lwip_httpd_handler.c
 *
 *  Created on: Nov 10, 2016
 *      Author: liyixiao
 */

#include "lwip/apps/httpd.h"

// ============================= Handle HTTP POST Requests =============================

// TODO: dummy
static char post_body[1024 * 1024];
static uint32_t post_len = 0;

#define EV3RT_UPLOAD_URI  "/upload"
#define HEADER_TYPE_APP   "Content-Type: ev3rt/app\r\n"
// TODO: support "Content-Disposition: inline"

/** Called when a POST request has been received. The application can decide
 * whether to accept it or not.
 *
 * @param connection Unique connection identifier, valid until httpd_post_end
 *        is called.
 * @param uri The HTTP header URI receiving the POST request.
 * @param http_request The raw HTTP request (the first packet, normally).
 * @param http_request_len Size of 'http_request'.
 * @param content_len Content-Length from HTTP header.
 * @param response_uri Filename of response file, to be filled when denying the
 *        request
 * @param response_uri_len Size of the 'response_uri' buffer.
 * @param post_auto_wnd Set this to 0 to let the callback code handle window
 *        updates by calling 'httpd_post_data_recved' (to throttle rx speed)
 *        default is 1 (httpd handles window updates automatically)
 * @return ERR_OK: Accept the POST request, data may be passed in
 *         another err_t: Deny the POST request, send back 'bad request'.
 */
err_t httpd_post_begin(void *connection, const char *uri, const char *http_request,
                       u16_t http_request_len, int content_len, char *response_uri,
                       u16_t response_uri_len, u8_t *post_auto_wnd) {
    if (strcmp(EV3RT_UPLOAD_URI, uri) != 0) {  // Only upload is supported
        syslog(LOG_ERROR, "HTTP POST URI is invalid.");
        return ERR_ARG;
    }
    if (memmem(http_request, http_request_len, HEADER_TYPE_APP, strlen(HEADER_TYPE_APP))) { // Receive an application

    } else {
        syslog(LOG_ERROR, "HTTP POST Content-Type is invalid.");
        return ERR_ARG;
    }
    post_len = 0;
    memcpy(post_body + post_len, http_request, http_request_len);
    post_len += http_request_len;
    return ERR_OK;
    //return ERR_ARG;
}

/** Called for each pbuf of data that has been received for a POST.
 * ATTENTION: The application is responsible for freeing the pbufs passed in!
 *
 * @param connection Unique connection identifier.
 * @param p Received data.
 * @return ERR_OK: Data accepted.
 *         another err_t: Data denied, http_post_get_response_uri will be called.
 */
err_t httpd_post_receive_data(void *connection, struct pbuf *p) {
    memcpy(post_body + post_len, p->payload, p->len);
    post_len += p->len;
    pbuf_free(p);
    return ERR_OK;
    //return ERR_ARG;
}

/** Called when all data is received or when the connection is closed.
 * The application must return the filename/URI of a file to send in response
 * to this POST request. If the response_uri buffer is untouched, a 404
 * response is returned.
 *
 * @param connection Unique connection identifier.
 * @param response_uri Filename of response file, to be filled when denying the request
 * @param response_uri_len Size of the 'response_uri' buffer.
 */
void httpd_post_finished(void *connection, char *response_uri, u16_t response_uri_len) {
    syslog(LOG_ERROR, "Recv POST finished, total %d bytes received.", post_len);
    post_body[post_len] = '\0';
    syslog(LOG_ERROR, "Recv data:\n%s", post_body);
}
