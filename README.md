
# A network-based FUSE filesystem using gRPC
##### Assignment 2 for CS798: Advanced Distributed Systems (Winter 2019) at UWaterloo

The filesystem mimics a very basic version of NFS protocol v1 using FUSE3 for moutning on the client-side and gRPC for requests transfers to the server. On top of the NFSv1 specs, the system has an added feature to COMMIT the write operations instead of making write persistant each time the file is changed on the client. The following is the highlights of the system: 

- **Asynchronous writes** to the client and server cache instead of hitting the disk on each write.
- **Server crash recovery** in case the server crashes or restarts. Since the asynchronous writes on the server gets lost, the client is responsible for re-transmitting the write requests again and then trying to commit.
- Minimalist **Fault tolerance** by way of caching writes before commit. The client blocks in case the server becomes non-respondent while tries to connect in an unbounded loop.


### usage:
1. Set the IP address and port for listening at an appropriate connection socket at `server/server.cc`. Also set the sub-directory in the server which needs to be mounted on the client system.
2. Set the IP address and port of the server in `client/client.cc`
3. `make` the source from the base directory of the repository.
4. Run `./server/server` to start the server
5. Run the client using `./client/client -f <mount-location>`
6. `make clean` to clean the complete repo from the base directory.

### Compatibility:
1. FUSE userspace filesystem v3.4.1
2. gRPC v1.17.2
3. gcc version 7.3.0

# authors:
Shreesha Addala and me. at University of Waterloo. 