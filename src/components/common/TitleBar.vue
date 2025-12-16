<!-- src/components/common/TitleBar.vue -->
<!-- 自定义标题栏 - 支持窗口拖动和控制按钮 -->
<script setup lang="ts">
import { ref, onMounted } from 'vue';
import { invoke } from '@tauri-apps/api/core';
import { IconClose, IconMinimize, IconMaximize, IconRestore } from '@/components/icons';

const isMaximized = ref(false);

onMounted(async () => {
  isMaximized.value = await invoke('is_window_maximized');
});

// 窗口控制函数
const minimizeWindow = async () => {
  await invoke('minimize_window');
};

const toggleMaximize = async () => {
  await invoke('toggle_maximize_window');
  isMaximized.value = await invoke('is_window_maximized');
};

const closeWindow = async () => {
  await invoke('close_window');
};
</script>

<template>
  <div class="titlebar" data-tauri-drag-region>
    <div class="titlebar-content">
      <div class="titlebar-icon">
        <svg width="20" height="20" viewBox="0 0 24 24" fill="currentColor">
          <path d="M12 2L2 7l10 5 10-5-10-5zM2 17l10 5 10-5M2 12l10 5 10-5" 
                fill="none" stroke="currentColor" stroke-width="2" 
                stroke-linecap="round" stroke-linejoin="round"/>
        </svg>
      </div>
      <span class="titlebar-title">Bangumi to Obsidian</span>
    </div>
    
    <div class="titlebar-buttons">
      <button 
        class="titlebar-button" 
        @click="minimizeWindow"
        title="最小化"
      >
        <IconMinimize :size="12" />
      </button>
      <button 
        class="titlebar-button" 
        @click="toggleMaximize"
        :title="isMaximized ? '还原' : '最大化'"
      >
        <IconRestore v-if="isMaximized" :size="12" />
        <IconMaximize v-else :size="12" />
      </button>
      <button 
        class="titlebar-button titlebar-button-close" 
        @click="closeWindow"
        title="关闭"
      >
        <IconClose :size="12" />
      </button>
    </div>
  </div>
</template>

<style scoped>
.titlebar {
  display: flex;
  justify-content: space-between;
  align-items: center;
  height: 32px;
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  color: white;
  user-select: none;
  -webkit-user-select: none;
  -webkit-app-region: drag;
}

.titlebar-content {
  display: flex;
  align-items: center;
  gap: 8px;
  padding-left: 12px;
  flex: 1;
}

.titlebar-icon {
  display: flex;
  align-items: center;
  justify-content: center;
  opacity: 0.9;
}

.titlebar-title {
  font-size: 13px;
  font-weight: 500;
  letter-spacing: 0.3px;
}

.titlebar-buttons {
  display: flex;
  height: 100%;
  -webkit-app-region: no-drag;
}

.titlebar-button {
  display: flex;
  align-items: center;
  justify-content: center;
  width: 46px;
  height: 100%;
  border: none;
  background: transparent;
  color: white;
  cursor: pointer;
  transition: background-color 0.15s ease;
}

.titlebar-button:hover {
  background: rgba(255, 255, 255, 0.1);
}

.titlebar-button:active {
  background: rgba(255, 255, 255, 0.2);
}

.titlebar-button-close:hover {
  background: #e81123;
}

.titlebar-button-close:active {
  background: #c50f1f;
}
</style>
