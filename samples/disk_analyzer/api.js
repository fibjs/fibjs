const fs = require('fs').promises;
const path = require('path');
const os = require('os');

// 全局变量
let rootNode = {
    name: '',
    size: 0,
    modified: new Date().toISOString(),
    isDirectory: true,
    children: [],
    expanded: false
};
const queue = [];
const maxConcurrentTasks = 20;
let activeTasks = 0;
let isTreeModified = false;

// 处理队列
const processQueue = async () => {
    if (queue.length === 0 || activeTasks >= maxConcurrentTasks) {
        return;
    }

    activeTasks++;
    while (queue.length > 0) {
        const { dirPath, node } = queue.shift();
        try {
            await readDirectoryTree(dirPath, node);
        } catch (error) {
            console.error(`Failed to read directory: ${dirPath}`, error);
        }
    }
    activeTasks--;
};

// 添加目录到队列
const addDirectoryToQueue = (dirPath, node) => {
    queue.push({ dirPath, node });
    processQueue();
};

// 读取目录树
const readDirectoryTree = async (dirPath, node) => {
    const children = await fs.readdir(dirPath);
    const existingChildrenNames = new Set(children);

    // Remove non-existing nodes
    node.children = node.children.filter(child => existingChildrenNames.has(child.name));

    // Add or update nodes
    for (const child of children) {
        const childPath = path.join(dirPath, child);
        var childStats;

        try {
            childStats = await fs.lstat(childPath); // Use lstat to check for symbolic links
        } catch (error) {
            console.error(`Failed to read file stats: ${childPath}`, error);
            continue;
        }

        let childNode = node.children.find(n => n.name === child);
        if (childNode && childNode.isDirectory !== childStats.isDirectory()) {
            node.children = node.children.filter(n => n.name !== child);
            childNode = null;
        }

        if (!childNode) {
            childNode = {};
            childNode.name = child;
            childNode.isDirectory = childStats.isDirectory();
            if (childNode.isDirectory) {
                childNode.size = 0;
                childNode.files = 0;
                childNode.children = [];
                childNode.expanded = false;
            }
            node.children.push(childNode);
        }

        childNode.modified = childStats.mtime.toISOString();

        if (childStats.isSymbolicLink()) {
            childNode.size = 0; // Set size to 0 for symbolic links
        } else if (childStats.isDirectory()) {
            addDirectoryToQueue(childPath, childNode);
        } else {
            childNode.size = childStats.size;
            childNode.files = 1;
        }

        isTreeModified = true;
    }
};

// 计算目录树的统计数据
const calculateTreeStats = (node) => {
    if (!node.children) {
        node.files = 1;
        return;
    }

    node.size = node.children.reduce((total, child) => {
        calculateTreeStats(child);
        return total + child.size;
    }, 0);

    node.files = node.children.reduce((total, child) => total + child.files, 0);
};

// 根据路径查找节点
const findNodeByPath = (node, path) => {
    if (path.length === 0) return node;

    const [current, ...restPath] = path;
    const child = node.children?.find(child => child.name === current);
    return child ? findNodeByPath(child, restPath) : null;
};

// 设置节点展开状态
const setNodeExpanded = (node, path, expanded) => {
    const pathSegments = path.split('/').filter(Boolean);
    const firstLevelDir = pathSegments[0];

    if (firstLevelDir !== rootNode.name) {
        console.error(`First-level directory "${firstLevelDir}" does not match rootNode name "${rootNode.name}"`);
        return;
    }

    const targetNode = findNodeByPath(node, pathSegments.slice(1));
    if (targetNode && targetNode.isDirectory) {
        targetNode.expanded = expanded;
    }
};

// 获取可见的目录树
const getVisibleTree = (node) => {
    if (node.isDirectory)
        return {
            ...node,
            children: node.expanded ? node.children.map(getVisibleTree) : []
        }
    else
        return node;
};

// 刷新数据
const refreshData = async () => {
    if (activeTasks > 0) return;

    const rootPath = path.resolve(os.homedir()); // 修改为你要读取的目录路径
    var rootStats;

    try {
        rootStats = await fs.lstat(rootPath); // Use lstat to check for symbolic links
        rootNode.name = path.basename(rootPath);
        rootNode.modified = rootStats.mtime.toISOString();
    } catch (error) {
        console.error(`Failed to read file stats: ${rootPath}`, error);
    }

    addDirectoryToQueue(rootPath, rootNode);
};

// 获取目录树数据
const fetchTreeData = () => {
    if (!rootNode) return null;

    if (isTreeModified) {
        calculateTreeStats(rootNode);
        sortTreeBySize(rootNode);
        isTreeModified = false;
    }

    return getVisibleTree(rootNode);
};

// 设置节点展开状态的 API
const setNodeExpandedAPI = (path, expanded) => {
    setNodeExpanded(rootNode, path, expanded);
};

// 生成简化输出
const generateSimplifiedOutput = (node, indent = 0) => {
    const indentation = ' '.repeat(indent);
    let output = `${indentation}${node.name} (size: ${node.size})\n`;
    if (node.children) {
        for (const child of node.children) {
            output += generateSimplifiedOutput(child, indent + 2);
        }
    }
    return output;
};

// 排序目录树
const sortTreeBySize = (node) => {
    if (node.children) {
        node.children.sort((a, b) => b.size - a.size);
        node.children.forEach(sortTreeBySize);
    }
};

// 初始化数据
refreshData();

module.exports = {
    fetchTreeData,
    setNodeExpanded: setNodeExpandedAPI,
    refreshData,
    isRefreshing: () => activeTasks > 0
};