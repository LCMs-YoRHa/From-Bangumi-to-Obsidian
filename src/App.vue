<!-- src/App.vue -->
<!-- 应用根组件 -->
<script setup lang="ts">
import { ref } from 'vue';
import TitleBar from '@/components/common/TitleBar.vue';
import StatusBar from '@/components/common/StatusBar.vue';
import CredentialsForm from '@/components/auth/CredentialsForm.vue';
import BatchImportTab from '@/components/tabs/BatchImportTab.vue';
import SingleImportTab from '@/components/tabs/SingleImportTab.vue';
import LocalProcessTab from '@/components/tabs/LocalProcessTab.vue';
import PrivacySettingsTab from '@/components/tabs/PrivacySettingsTab.vue';
import { IconBatch, IconSingle, IconSettings, IconPrivacy } from '@/components/icons';

type TabName = 'batch' | 'single' | 'local' | 'privacy';

const activeTab = ref<TabName>('batch');

const tabs = [
  { name: 'batch' as const, label: '批量导入', icon: IconBatch, component: BatchImportTab },
  { name: 'single' as const, label: '单个导入', icon: IconSingle, component: SingleImportTab },
  { name: 'local' as const, label: '本地管理', icon: IconSettings, component: LocalProcessTab },
  { name: 'privacy' as const, label: '隐私设置', icon: IconPrivacy, component: PrivacySettingsTab },
];
</script>

<template>
  <div class="app-container">
    <!-- 自定义标题栏 -->
    <TitleBar />

    <main class="app-main">
      <!-- 认证区域 -->
      <section class="auth-section">
        <CredentialsForm />
      </section>

      <!-- 功能选项卡 -->
      <section class="tabs-section">
        <div class="tabs-header">
          <button
            v-for="tab in tabs"
            :key="tab.name"
            class="tab-button"
            :class="{ active: activeTab === tab.name }"
            @click="activeTab = tab.name"
          >
            <component :is="tab.icon" :size="16" class="tab-icon" />
            <span>{{ tab.label }}</span>
          </button>
        </div>

        <div class="tab-content">
          <component
            :is="tabs.find(t => t.name === activeTab)?.component"
          />
        </div>
      </section>
    </main>

    <StatusBar />
  </div>
</template>

<style>
/* 全局样式 */
* {
  box-sizing: border-box;
}

body {
  margin: 0;
  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen,
    Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;
  background: #f0f2f5;
  color: #333;
  overflow: hidden;
}

#app {
  height: 100vh;
  display: flex;
  flex-direction: column;
}

/* 滚动条样式 */
::-webkit-scrollbar {
  width: 8px;
  height: 8px;
}

::-webkit-scrollbar-track {
  background: #f1f1f1;
  border-radius: 4px;
}

::-webkit-scrollbar-thumb {
  background: #c1c1c1;
  border-radius: 4px;
}

::-webkit-scrollbar-thumb:hover {
  background: #a8a8a8;
}
</style>

<style scoped>
.app-container {
  display: flex;
  flex-direction: column;
  height: 100vh;
  overflow: hidden;
}

.app-main {
  flex: 1;
  overflow-y: auto;
  padding: 16px 20px;
  display: flex;
  flex-direction: column;
  gap: 16px;
}

.auth-section {
  flex-shrink: 0;
}

.tabs-section {
  flex: 1;
  display: flex;
  flex-direction: column;
  background: white;
  border-radius: 8px;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.08);
  overflow: hidden;
  min-height: 0;
}

.tabs-header {
  display: flex;
  border-bottom: 1px solid #e8e8e8;
  background: #fafafa;
  flex-shrink: 0;
}

.tab-button {
  flex: 1;
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 6px;
  padding: 12px 16px;
  border: none;
  background: transparent;
  font-size: 0.875rem;
  font-weight: 500;
  color: #666;
  cursor: pointer;
  transition: all 0.2s ease;
  position: relative;
}

.tab-button:hover {
  color: #333;
  background: #f0f0f0;
}

.tab-button.active {
  color: #667eea;
  background: white;
}

.tab-button.active::after {
  content: '';
  position: absolute;
  bottom: 0;
  left: 0;
  right: 0;
  height: 2px;
  background: linear-gradient(90deg, #667eea 0%, #764ba2 100%);
}

.tab-icon {
  flex-shrink: 0;
}

.tab-content {
  flex: 1;
  overflow-y: auto;
  min-height: 0;
}
</style>
