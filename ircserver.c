#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h> // Pour select()

#define PORT 8000
#define MAX_CLIENTS 30 // Le nombre maximum de clients qu'on gère
#define BUFLEN 512

void stop(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    int listen_sockfd; // La socket d'écoute principale
    int client_sockets[MAX_CLIENTS]; // Tableau pour stocker les sockets des clients
    int i;

    // --- Initialisation du tableau des clients ---
    // On met tout à 0 (0 est un fd non valide, c'est bien)
    for (i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = 0;
    }

    // --- Création de la socket d'écoute ---
    listen_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sockfd == -1) {
        stop("socket()");
    }
    printf("Socket d'écoute créée (fd = %d)\n", listen_sockfd);

    // --- Configuration de l'adresse du serveur ---
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY; // Écoute sur toutes les IP

    // --- Bind et Listen ---
    if (bind(listen_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        stop("bind()");
    }
    if (listen(listen_sockfd, 5) == -1) {
        stop("listen()");
    }

    printf("Serveur IRC en écoute sur le port %d\n", PORT);

    // ... La boucle principale avec select() viendra ici ...

    fd_set read_fds; // Ensemble des sockets à lire
    int max_fd;      // La socket avec le numéro le plus élevé

    while (1) {
        // 1. Préparer l'ensemble read_fds pour select()
        FD_ZERO(&read_fds); // On réinitialise l'ensemble

        // 2. Ajouter la socket d'écoute à l'ensemble
        FD_SET(listen_sockfd, &read_fds);
        max_fd = listen_sockfd;

        // 3. Ajouter toutes les sockets client valides à l'ensemble
        for (i = 0; i < MAX_CLIENTS; i++) {
            int client_fd = client_sockets[i];
            
            if (client_fd > 0) {
                FD_SET(client_fd, &read_fds);
            }
            
            // On met à jour le max_fd pour select()
            if (client_fd > max_fd) {
                max_fd = client_fd;
            }
        }

        // 4. Appeler select() - C'est ici que le serveur ATTEND
        // select() va modifier read_fds pour ne garder que les sockets
        // qui ont de l'activité.
        int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        if (activity < 0) {
            perror("select()");
            continue; // On repart dans la boucle
        }

        // 5. Gérer l'activité - Une socket est prête !
        
        // 5a. Est-ce la socket d'écoute ? (Nouveau client)
        if (FD_ISSET(listen_sockfd, &read_fds)) {
            // C'est une nouvelle connexion !
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int new_sockfd = accept(listen_sockfd, (struct sockaddr *)&client_addr, &client_len);
            
            if (new_sockfd < 0) {
                perror("accept()");
            } else {
                printf("Nouveau client connecté (fd = %d)\n", new_sockfd);
                
                // On ajoute ce nouveau client à notre tableau
                for (i = 0; i < MAX_CLIENTS; i++) {
                    if (client_sockets[i] == 0) {
                        client_sockets[i] = new_sockfd;
                        break;
                    }
                }
            }
        }
        
        // 5b. Est-ce un client qui parle ?
        char buffer[BUFLEN + 1];
        for (i = 0; i < MAX_CLIENTS; i++) {
            int sender_fd = client_sockets[i];

            if (sender_fd > 0 && FD_ISSET(sender_fd, &read_fds)) {
                // Ce client (sender_fd) a envoyé un message
                memset(buffer, 0, BUFLEN + 1);
                int n = recv(sender_fd, buffer, BUFLEN, 0);

                if (n <= 0) {
                    // Erreur ou déconnexion
                    printf("Client (fd = %d) déconnecté.\n", sender_fd);
                    close(sender_fd);
                    client_sockets[i] = 0; // On libère la place
                } else {
                    // --- LOGIQUE IRC : Transférer à tous les autres ---
                    printf("Message de (fd = %d): %s\n", sender_fd, buffer);
                    
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        int dest_fd = client_sockets[j];
                        
                        // Si c'est un client valide ET ce n'est pas l'expéditeur
                        if (dest_fd > 0 && dest_fd != sender_fd) {
                            if (send(dest_fd, buffer, n, 0) == -1) {
                                perror("send() broadcast");
                            }
                        }
                    }
                }
            }
        } // Fin de la boucle sur les clients

    } // Fin du while(1)
    
    return 0;
}