<!-- src/components/common/AppHeader.vue -->
<!-- 应用顶部标题栏 -->
<script setup lang="ts">
defineProps<{
  title?: string;
}>();
</script>

<template>
  <header class="app-header">
    <div class="header-content">
      <h1 class="app-title">{{ title || 'Bangumi to Obsidian' }}</h1>
      <p class="app-subtitle">将 Bangumi 收藏导出为 Obsidian Markdown</p>
    </div>
    <div class="header-actions">
      <slot name="actions" />
    </div>
  </header>
</template>

<style scoped>
.app-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 16px 24px;
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  color: white;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.15);
}

.header-content {
  display: flex;
  flex-direction: column;
  gap: 4px;
}

.app-title {
  margin: 0;
  font-size: 1.5rem;
  font-weight: 600;
}

.app-subtitle {
  margin: 0;
  font-size: 0.875rem;
  opacity: 0.9;
}

.header-actions {
  display: flex;
  gap: 8px;
}
</style>
