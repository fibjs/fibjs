// FILE: index.jsx
import React, { useState, useEffect } from 'react';
import { createRoot } from 'react-dom/client';
import './styles.css'; // 引入样式文件
import TreeNode from './TreeNode'; // 引入 TreeNode 组件

const formatSize = (size) => {
  // 格式化大小的函数
  if (size < 1024) return `${size} B`;
  if (size < 1024 * 1024) return `${(size / 1024).toFixed(2)} KB`;
  if (size < 1024 * 1024 * 1024) return `${(size / (1024 * 1024)).toFixed(2)} MB`;
  return `${(size / (1024 * 1024 * 1024)).toFixed(2)} GB`;
};

const App = () => {
  const [treeData, setTreeData] = useState(null);

  const refreshData = () => {
      window.app.refreshData();
  };

  const handleRefresh = async () => {
    try {
      const data = await window.app.fetchTreeData();
      setTreeData(data);
    } catch (error) {
      console.error('Error fetching tree data:', error);
    }
  };

  useEffect(() => {
    handleRefresh();
    const intervalId = setInterval(handleRefresh, 500); // 每500毫秒刷新一次

    return () => clearInterval(intervalId); // 清除定时器
  }, []);

  return (
    <div className="container">
      <div className="info">
        Disk Analyzer
        <button onClick={refreshData} className="refresh-button">
          &#x21bb; {/* Unicode for refresh icon */}
        </button>
      </div>
      <div className="content">
        <div className="left">
          <div className="tree-header">
            <span className="tree-header-name">Name</span>
            <div className="tree-header-details">
              <span className="tree-header-size">Size</span>
              <span className="tree-header-contents">Contents</span>
              <span className="tree-header-modified">Modified</span>
            </div>
          </div>
          {treeData ? <TreeNode node={treeData} formatSize={formatSize} onRefresh={handleRefresh} /> : <p>Loading...</p>}
        </div>
      </div>
    </div>
  );
};

const container = document.getElementById('root');
const root = createRoot(container);
root.render(<App />);