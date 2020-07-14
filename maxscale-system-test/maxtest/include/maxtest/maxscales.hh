#pragma once

#include <string>
#include <thread>
#include <vector>

#include <maxtest/mariadb_func.hh>
#include <maxtest/mariadb_nodes.hh>
#include <maxtest/nodes.hh>

class Maxscales: public Nodes
{
public:
    enum service
    {
        RWSPLIT,
        READCONN_MASTER,
        READCONN_SLAVE
    };

    Maxscales(const char *pref, const char *test_cwd, bool verbose,
              const std::string& network_config);

    bool setup() override;

    int read_env();

    /**
     * @brief rwsplit_port RWSplit service port
     */
    int rwsplit_port[256];

    /**
     * @brief readconn_master_port ReadConnection in master mode service port
     */
    int readconn_master_port[256];

    /**
     * @brief readconn_slave_port ReadConnection in slave mode service port
     */
    int readconn_slave_port[256];

    /**
     * @brief Get port number of a MaxScale service
     *
     * @param type Type of service
     * @param m    MaxScale instance to use
     *
     * @return Port number of the service
     */
    int port(enum service type = RWSPLIT, int m = 0) const;

    /**
     * @brief binlog_port binlog router service port
     */
    int binlog_port[256];

    /**
     * @brief conn_rwsplit  MYSQL connection struct to RWSplit service
     */
    MYSQL* conn_rwsplit[256];

    /**
     * @brief conn_master   MYSQL connection struct to ReadConnection in master mode service
     */
    MYSQL* conn_master[256];

    /**
     * @brief conn_slave MYSQL connection struct to ReadConnection in slave mode service
     */
    MYSQL* conn_slave[256];

    /**
     * @brief routers Array of 3 MYSQL handlers which contains copies of conn_rwsplit, conn_master, conn_slave
     */
    MYSQL* routers[256][3];

    /**
     * @brief ports of 3 int which contains copies of rwsplit_port, readconn_master_port, readconn_slave_port
     */
    int ports[256][3];

    /**
      * @brief maxscale_cnf full name of Maxscale configuration file
      */
    char * maxscale_cnf[256];

    /**
      * @brief maxscale_log_dir name of log files directory
      */
    char * maxscale_log_dir[256];

    /**
      * @brief maxscale_lbinog_dir name of binlog files (for binlog router) directory
      */
    char * maxscale_binlog_dir[256];

    /**
     * @brief N_ports Default number of routers
     */
    int N_ports[256];

    /**
     * @brief test_dir path to test application
     */
    char test_dir[4096];

    bool ssl;

    /**
     * @brief ConnectMaxscale   Opens connections to RWSplit, ReadConn master and ReadConn slave Maxscale
     * services
     * Opens connections to RWSplit, ReadConn master and ReadConn slave Maxscale services
     * Connections stored in maxscales->conn_rwsplit[0], maxscales->conn_master[0] and
     * maxscales->conn_slave[0] MYSQL structs
     * @return 0 in case of success
     */
    int connect_maxscale(int m = 0, const std::string& db = "test");
    int connect(int m = 0, const std::string& db = "test")
    {
        return connect_maxscale(m, db);
    }

    /**
     * @brief CloseMaxscaleConn Closes connection that were opened by ConnectMaxscale()
     * @return 0
     */
    int close_maxscale_connections(int m = 0);
    int disconnect(int m = 0)
    {
        return close_maxscale_connections(m);
    }

    /**
     * @brief ConnectRWSplit    Opens connections to RWSplit and store MYSQL struct in
     * maxscales->conn_rwsplit[0]
     * @return 0 in case of success
     */
    int connect_rwsplit(int m = 0, const std::string& db = "test");

    /**
     * @brief ConnectReadMaster Opens connections to ReadConn master and store MYSQL struct in
     * maxscales->conn_master[0]
     * @return 0 in case of success
     */
    int connect_readconn_master(int m = 0, const std::string& db = "test");

    /**
     * @brief ConnectReadSlave Opens connections to ReadConn slave and store MYSQL struct in
     * maxscales->conn_slave[0]
     * @return 0 in case of success
     */
    int connect_readconn_slave(int m = 0, const std::string& db = "test");

    /**
     * @brief OpenRWSplitConn   Opens new connections to RWSplit and returns MYSQL struct
     * To close connection mysql_close() have to be called
     * @return MYSQL struct
     */
    MYSQL* open_rwsplit_connection(int m = 0, const std::string& db = "test")
    {
        return open_conn(rwsplit_port[m], IP[m], user_name, password, ssl);
    }

    /**
     * Get a readwritesplit Connection
     */
    Connection rwsplit(int m = 0, const std::string& db = "test")
    {
        return Connection(IP[m], rwsplit_port[m], user_name, password, db, ssl);
    }

    /**
     * Get a Connection to a specific port
     */
    Connection get_connection(int port, int m = 0, const std::string& db = "test")
    {
        return Connection(IP[m], port, user_name, password, db, ssl);
    }

    /**
     * @brief OpenReadMasterConn    Opens new connections to ReadConn master and returns MYSQL struct
     * To close connection mysql_close() have to be called
     * @return MYSQL struct
     */
    MYSQL* open_readconn_master_connection(int m = 0)
    {
        return open_conn(readconn_master_port[m],
                         IP[m],
                         user_name,
                         password,
                         ssl);
    }

    /**
     * Get a readconnroute master Connection
     */
    Connection readconn_master(int m = 0, const std::string& db = "test")
    {
        return Connection(IP[m], readconn_master_port[m], user_name, password, db, ssl);
    }

    /**
     * @brief OpenReadSlaveConn    Opens new connections to ReadConn slave and returns MYSQL struct
     * To close connection mysql_close() have to be called
     * @return  MYSQL struct
     */
    MYSQL* open_readconn_slave_connection(int m = 0)
    {
        return open_conn(readconn_slave_port[m],
                         IP[m],
                         user_name,
                         password,
                         ssl);
    }

    /**
     * Get a readconnroute slave Connection
     */
    Connection readconn_slave(int m = 0, const std::string& db = "test")
    {
        return Connection(IP[m], readconn_slave_port[m], user_name, password, db, ssl);
    }

    /**
     * @brief CloseRWSplit Closes RWplit connections stored in maxscales->conn_rwsplit[0]
     */
    void close_rwsplit(int m = 0)
    {
        mysql_close(conn_rwsplit[m]);
        conn_rwsplit[m] = NULL;
    }

    /**
     * @brief CloseReadMaster Closes ReadConn master connections stored in maxscales->conn_master[0]
     */
    void close_readconn_master(int m = 0)
    {
        mysql_close(conn_master[m]);
        conn_master[m] = NULL;
    }

    /**
     * @brief CloseReadSlave Closes ReadConn slave connections stored in maxscales->conn_slave[0]
     */
    void close_readconn_slave(int m = 0)
    {
        mysql_close(conn_slave[m]);
        conn_slave[m] = NULL;
    }

    /**
     * @brief restart_maxscale Issues 'service maxscale restart' command
     */
    int restart_maxscale(int m = 0);
    int restart(int m = 0)
    {
        return restart_maxscale(m);
    }

    /**
     * @brief alias for restart_maxscale
     */
    int start_maxscale(int m = 0);

    int start(int m = 0)
    {
        return start_maxscale(m);
    }

    /**
     * @brief stop_maxscale Issues 'service maxscale stop' command
     */
    int stop_maxscale(int m = 0);
    int stop(int m = 0)
    {
        return stop_maxscale(m);
    }

    // Helper for stopping all maxscales
    void stop_all()
    {
        std::vector<std::thread> thr;

        for (int i = 0; i < N; i++)
        {
            thr.emplace_back([this, i]() {
                                 stop(i);
                             });
        }

        for (auto& a : thr)
        {
            a.join();
        }
    }
    /**
     * Execute a MaxCtrl command
     *
     * @param cmd  Command to execute, without the `maxctrl` part
     * @param m    MaxScale node to execute the command on
     * @param sudo Run the command as root
     *
     * @return The exit code and output of MaxCtrl
     */
    SshResult maxctrl(const std::string& cmd, int m = 0, bool sudo = true)
    {
        return ssh_output("maxctrl " + cmd, m, sudo);
    }

    /**
     * @brief get_maxscale_memsize Gets size of the memory consumed by Maxscale process
     * @param m Number of Maxscale node
     * @return memory size in kilobytes
     */
    long unsigned get_maxscale_memsize(int m = 0);

    /**
     * @brief Get the set of labels that are assigned to server @c name
     *
     * @param name The name of the server
     *
     * @param m Number of Maxscale node
     *
     * @return A set of string labels assigned to this server
     */
    StringSet get_server_status(const std::string& name, int m = 0);

    /**
     * Wait until the monitors have performed at least one monitoring operation
     *
     * The function waits until all monitors have performed at least one monitoring cycle.
     *
     * @param intervals The number of monitor intervals to wait
     * @param m Number of Maxscale node
     */
    void wait_for_monitor(int intervals = 2, int m = 0);

    /**
     * @brief use_valrind if true Maxscale will be executed under Valgrind
     */
    bool use_valgrind;

    /**
     * @brief use_callgrind if true Maxscale will be executed under Valgrind with
     * --callgrind option
     */
    bool use_callgrind;

    /**
     * @brief valgring_log_num Counter for Maxscale restarts to avoid Valgrind log overwriting
     */
    int valgring_log_num;

};

class TestConnections;

/**
 * Contains information about one server as seen by MaxScale.
 */
struct ServerInfo
{
    static constexpr uint RUNNING = (1 << 0);
    static constexpr uint MASTER = (1 << 1);
    static constexpr uint SLAVE = (1 << 2);
    static constexpr uint RELAY = (1 << 3);

    static std::string status_to_string(uint status);
    std::string        status_to_string() const;

    void status_from_string(const std::string& source);

    std::string name;       /**< Server name */
    uint        status {0}; /**< Status bitfield */
};

/**
 * Contains information about multiple servers as seen by MaxScale.
 */
class ServersInfo
{
public:
    void add(const ServerInfo& info);
    const ServerInfo& get(size_t i);
    size_t size() const;

private:
    std::vector<ServerInfo> m_servers;
};

class MaxScale
{
public:
    MaxScale(TestConnections& tester, int node_ind);

    /**
     * Wait for monitors to tick.
     *
     * @param ticks The number of monitor ticks to wait
     */
    void wait_monitor_ticks(int ticks = 1);

    /**
     * Get servers info.
     *
     * @return Server info object
     */
    ServersInfo get_servers();

    /**
     * Check that server status is as expected. Increments global error counter if differences found.
     *
     * @param expected_status Expected server statuses. Each status should be a bitfield of values defined
     * in the ServerInfo-class.
     */
    void check_servers_status(std::vector<uint> expected_status);

private:
    TestConnections& m_tester;    /**< Main tester object */
    int m_node_ind {-1};          /**< Node index of this MaxScale */

    std::string m_rest_user {"admin"};
    std::string m_rest_pw {"mariadb"};
    std::string m_rest_ip {"127.0.0.1"};
    std::string m_rest_port {"8989"};

    Nodes::SshResult curl_rest_api(const std::string& path);
};