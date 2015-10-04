#ifndef LIBUSEFUL_OAUTH_H
#define LIBUSEFUL_OAUTH_H

void OAuthDeviceLogin(char *LoginURL, char *ClientID, char *Scope, char **DeviceCode, char **UserCode, char **NextURL);
void OAuthDeviceGetToken(char *TokenURL, char *ClientID, char *ClientSecret, char *DeviceCode, char **AccessToken, char **RefreshToken);
void OAuthDeviceRefreshToken(char *TokenURL, char *ClientID, char *ClientSecret, char *RefreshToken, char **AccessToken);

#endif

